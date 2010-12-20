#ifndef RAZORBARTASK_CPP
#define RAZORBARTASK_CPP

#include "razorbartask.h"
#include "razortask.h"
#include "razor.h"
#include "razorplugin.h"


/**
 * @file razorbartask.cpp
 * @author Christopher "VdoP" Regali
 * @brief implements Razorbartask and Razorbartaskentry
 */


/**
 * @brief constructor
 */
RazorBarTask::RazorBarTask(RazorPlugin* _owner)
{
    qDebug() << "Razorbartask initializing...";

    owner=  _owner;

    Layout = new QHBoxLayout;
    setScaledContents(true);
    Layout->setSpacing(0);
    Layout->setMargin(0);

//    if (!Razor::getInstance().get_looknfeel()->getString("taskbar_background").isEmpty())
//    {
//        setPixmap((QPixmap)(Razor::getInstance().get_looknfeel()->getPath() +  Razor::getInstance().get_looknfeel()->getString("taskbar_background")));
//    }

    makeUp();
}

/**
 * @brief updates the focused window and makes the entries look accordingly also sets the glow for windows that require attention
 */
void RazorBarTask::updateFocus()
{
    active = Razor::getInstance().get_Xfitman()->getActiveAppWindow();
    for (int i=0; i < taskMap.values().count(); i++)
        taskMap.values().at(i)->setChecked(false);
    if (taskMap.contains(active))
        taskMap.value(active)->setChecked(true);

    //glow for windows that want attention
    for (int i=0; i < taskMap.values().count(); i++)
      if (Razor::getInstance().get_Xfitman()->requiresAttention(taskMap.keys().at(i)))
	taskMap.values().at(i)->focusGlow();



}

/**
 * @brief this gives the button the glow it also gets when is has focus
 */

void RazorBarTaskEntry::focusGlow()
{
  QToolButton::focusInEvent(NULL);
}




/**
 * @brief adds this to our Layout and resets it
 */

void RazorBarTask::addItem(QWidget* _newtaskentry)
{
    Q_UNUSED(_newtaskentry);
}


/**
 * @brief makes up our style
 */
void RazorBarTask::makeUp()
{
    //setLayout(Layout);
    show();
}

/**
 * @brief destructor
 */
RazorBarTask::~RazorBarTask()
{

}

/**
 * @brief updates our shown tasks according to the tasklist
 */
void RazorBarTask::updateTasks(QMap<Window, RazorTask*>* _list)
{
    //we need to get gui and backbone in sync so first get the actual clientlist we dont care how its updated or stuff
    QMapIterator<Window, RazorBarTaskEntry*> iter (taskMap);
    //now at first we purge the list
    while (iter.hasNext())
    {
        iter.next();

        if (!_list->contains(iter.key()))
        {
            //this means we have found an unused but existing Razorbartaskentry.. try to kick it :)
            RazorBarTaskEntry* todel = iter.value();
            //remove it from layout, so the user doenst see shit going on
            layout()->removeWidget(todel);
            //now remove it from our shadowlayout
            Layout->removeWidget(todel);
            //then remove his utter existence!
            delete todel;
            //now remove from the Map
            taskMap.remove(iter.key());
        }
    }

    //after purging we create the missing items
    QMapIterator<Window, RazorTask*> clientIter (*_list);
    while (clientIter.hasNext())
    {
        clientIter.next();
        if (!taskMap.contains(clientIter.key()))
        {
            //prepare it
            RazorBarTaskEntry* newitem = new RazorBarTaskEntry(clientIter.value(), this);
            //add it to our internal list
            taskMap[clientIter.key()] = newitem;

            //add it to layout
            Layout->addWidget(newitem,0,Qt::AlignLeft);
            setLayout(Layout);
        }
    }

    makeUp();

    //if anything changed resize the buttons!
    /*
     * you may ask why we use a new iterator here, when we could just move with iter.toFront();
     * this is not possible, cause iter still has the items of the unmodified list cached.
     * if we use it, we possibly would crash razor!
     */

    QMapIterator<Window, RazorBarTaskEntry*> newiter (taskMap);
    while (newiter.hasNext())
    {
        newiter.next();
        taskMap.value(newiter.key())->makeUp();
    }
}



/**
 * @brief destructor
 */
RazorBarTaskEntry::~RazorBarTaskEntry()
{

}


/**
 * @brief constructor
 */
RazorBarTaskEntry::RazorBarTaskEntry(RazorTask* _linkedtask, RazorBarTask* _owner)
{
    connect(this, SIGNAL(clicked(bool)),this, SLOT(doAction(bool)));
    owner=_owner;
    linkedTask = _linkedtask;
    //maybe we can use this in the future to indicate the active window.. but iam too lazy to implement gui-funstuff right now
    setCheckable(true);

    //setup the text
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    if (linkedTask->getIcon(qpm))
    {
        setIcon(qpm);
    }
    setToolTip(linkedTask->getTitle());
    //make us look good
    qDebug()<< "Razorbartaskentry " << toolTip() << "still there before makeup";
    makeUp();
    show();
}

/**
 * @brief handles the clicks and does the manipulation of the linked task
 */
void RazorBarTaskEntry::doAction(bool _checked)
{
    Q_UNUSED(_checked);
    qDebug() << "Razortaskbarentry: ACTION!";
    owner->updateFocus();

    if (!linkedTask->isHidden() && linkedTask->isActive() )
        linkedTask->toogleMin();
    else if (!linkedTask->isHidden() && !linkedTask->isActive() )
        linkedTask->raiseMe();
    else if (linkedTask->isHidden())
        linkedTask->toogleMin();
}



/**
 * @brief this makes up the style of the single item
 */
void RazorBarTaskEntry::makeUp()
{

    //set width - first get the count of the entries
    int count = owner->getEntryCount();
    //we dont want the buttons to get too fat so if there are less than 3 buttons - assume there are 3 anyway
    if (count < 3)
        count = 3;
    //leave some space for deco but else just use all the space we get
    setFixedWidth(owner->width()/count);


    setFixedHeight(owner->height()-5);
    //setPixmap((QPixmap)(Razor::getInstance().get_looknfeel()->getPath() +  Razor::getInstance().get_looknfeel()->getValue("task_background")));
    QString name = QApplication::fontMetrics().elidedText(linkedTask->getTitle(), Qt::ElideRight, width()-width()/3);
    if (linkedTask->isHidden())
    {
        name="["+name+"]";
    }
    setText(name);
}




#endif
