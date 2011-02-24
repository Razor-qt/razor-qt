#ifndef RAZORTASK_CPP
#define RAZORTASK_CPP

/**
 * @file razortask.cpp
 * @brief implements the class Razortask
 * @author Christopher "VdoP" Regali
 */

#include "../panel/razorpanelplugin.h"
#include "razorbartask.h"
#include <QtCore/QDebug>
#include <razorqt/xfitman.h>
#include "razortask.h"

EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorTaskManager)
/*
RazorPanelPlugin* init(RazorPanel* panel, QWidget* parent, const QString & name)
{
    RazorTaskManager * ret = new RazorTaskManager(panel, name, parent);//, name);
    Q_ASSERT(ret);
    return ret;
}
*/

/**
 * @brief constructor
 */
RazorTask::RazorTask(Window _id, int _screen)
{
    client=_id;
    onScreen = _screen;
    hasIcon = false;
    qDebug() << "getting name";
    title=xfitMan()->getName(client);
    qDebug() << "getting icon";
    if (xfitMan()->getClientIcon(client,clientIcon))
        hasIcon = true;
    qDebug() << "getting desktop";
    desktop=xfitMan()->getWindowDesktop(_id);
    qDebug() << "getting hidden";
    hidden=xfitMan()->isHidden(_id);
}

/**
 * @brief returns the task-title
 */
QString RazorTask::getTitle()
{
    title = xfitMan()->getName(client);
    return title;
}

/**
 * @brief raises the window
 */
void RazorTask::raiseMe()
{
    xfitMan()->raiseWindow(client);
}



/**
 * @brief requests Xfitman to toggle minimalize-flag for this tasks window!
 */
void RazorTask::toogleMin()
{
//    hidden = Razor::getInstance().get_Xfitman()->isHidden(client);
//    qDebug() << "Razortask::toggleMin, hidden:" << hidden;
//    Razor::getInstance().get_Xfitman()->setClientStateFlag(client,"net_wm_state_hidden",2);
//    hidden = !hidden;
//    qDebug() << "Razortask::toggleMin, hidden:" << hidden;
//    if (!hidden)
//        Razor::getInstance().get_Xfitman()->raiseWindow(client);
}

/**
 * @brief returns if the window has hiddenflag turned on
 */
bool RazorTask::isHidden()
{
    hidden = xfitMan()->isHidden(client);
    return hidden;
}

/**
 * @brief returns true if task has inputfocus
 */
bool RazorTask::isActive()
{
    return (xfitMan()->getActiveAppWindow()==client);
}


/**
 * @brief requests Xfitman to set the window to fullscreen (UNTESTED!)
 */
void RazorTask::setFullscreen()
{
    //first remove hidden-flag so make us visible / unminimize
    xfitMan()->setClientStateFlag(client,"net_wm_state_hidden",0);
    //then add the fullscreen-flag
    xfitMan()->setClientStateFlag(client,"net_wm_state_fullscreen",1);
}



/**
 * @brief destructor
 */
RazorTask::~RazorTask()
{

}

/**
 * @brief gets the client icon, if we have one!
 */
bool RazorTask::getIcon(QPixmap& _pixm)
{
    qDebug() << "Has this client an Icon?" <<  hasIcon << title;
    if (hasIcon)
        _pixm = clientIcon;
    return hasIcon;
}

/**
 * @brief handles the X events and sets client - gui states
 */
void RazorTaskManager::handleXEvent(XEvent* _event)
{
    //destroy or create windows? thats whats interesting.. we dont care about the rest!
    //so we just fetch "propertynotify" events
    if (_event->type == PropertyNotify)
        updateMap();
}

/**
 * @brief constructor
 */
RazorTaskManager::RazorTaskManager(RazorPanel* panel, const QString& configId, QWidget *parent)
    : RazorPanelPlugin(panel, configId, parent)
{
    setObjectName("TaskBar");
    setWindowTitle(tr("Task Bar"));

    qDebug() << "Razortaskmanager init";
    //now we setup our gui element
    gui = new RazorBarTask(this, panel);

    //now we need an updated map for the clients running
    updateMap();

    qDebug() << "Razortaskmanager updateMap done";
    //now we add our taskbar to the panel
    // it's aligned to left and it should occypy all free space
    // in the panel.
    // TODO: it doesn't work with sizeHints and with the stretch =1 too...

    addWidget(gui);
    qDebug() << "Razortaskmanager added widget";
    //we need the events so we can process the tasks correctly

//    Razor::getInstance().get_events()->registerCallback(this);

//    qDebug() << "Razortaskmanager callback registered";
    connect(panel, SIGNAL(x11PropertyNotify(XEvent*)), this, SLOT(handleXEvent(XEvent*)));
}
/**
 * @brief updates our clientmap
 */
void RazorTaskManager::updateMap()
{
    QList<Window> tmp = xfitMan()->getClientList();

    //first we need to get rid of tasks that got closed
    QMapIterator<Window,RazorTask*> iter(clientList);
    while (iter.hasNext())
    {
        iter.next();
        if (!tmp.contains(iter.key()))
        {
            //  qDebug() << "DELTHIS!";
            //get the pointer
            RazorTask* deltask = iter.value();
            //remove the link from the list
            clientList.remove(iter.key());
            //free the heap
            delete deltask;
        }
    }


    //now we got the window-ids and we got the count so we get through all of them
    for (int i = 0; i < tmp.count(); i++)
    {
        //add new clients to the list making new entries for the new windows
        if (!clientList.contains(tmp.at(i)) && xfitMan()->acceptWindow(tmp.at(i)))
        {
            RazorTask* rtask = new RazorTask(tmp.at(i), xfitMan()->getWindowDesktop(tmp.at(i)));
            qDebug() << "title: " <<rtask->getTitle();
            clientList[tmp.at(i)]=rtask;
        }
    }

    //then update the stuff in our gui
    gui->updateTasks(&clientList);
    gui->updateFocus();
}


/**
 * @brief destructor
 */
RazorTaskManager::~RazorTaskManager()
{
    delete xfitMan();
    for (int i = 0; i < clientList.values().count(); i++)
        delete clientList.values().at(i);
}



#endif
