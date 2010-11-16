#ifndef RAZORTASK_CPP
#define RAZORTASK_CPP

/**
 * @file razortask.cpp
 * @brief implements the class Razortask
 * @author Christopher "VdoP" Regali
 */

#include "razortask.h"
#include "razor.h"

/**
 * @brief constructor
 */
RazorTask::RazorTask(Window _id, int _screen)
{
    client=_id;
    onScreen = _screen;
    hasIcon = false;
    qDebug() << "getting name";
    title=Razor::getInstance().get_Xfitman()->getName(client);
    qDebug() << "getting icon";
    if (Razor::getInstance().get_Xfitman()->getClientIcon(client,clientIcon))
        hasIcon = true;
    qDebug() << "getting desktop";
    desktop=Razor::getInstance().get_Xfitman()->getWindowDesktop(_id);
    qDebug() << "getting hidden";
    hidden=Razor::getInstance().get_Xfitman()->isHidden(_id);
}

/**
 * @brief returns the task-title
 */
QString RazorTask::getTitle()
{
    title = Razor::getInstance().get_Xfitman()->getName(client);
    return title;
}

/**
 * @brief raises the window
 */
void RazorTask::raiseMe()
{
    Razor::getInstance().get_Xfitman()->raiseWindow(client);
}



/**
 * @brief requests Xfitman to toggle minimalize-flag for this tasks window!
 */
void RazorTask::toogleMin()
{
    hidden = Razor::getInstance().get_Xfitman()->isHidden(client);
    qDebug() << "Razortask::toggleMin, hidden:" << hidden;
    Razor::getInstance().get_Xfitman()->setClientStateFlag(client,"net_wm_state_hidden",2);
    hidden = !hidden;
    qDebug() << "Razortask::toggleMin, hidden:" << hidden;
    if (!hidden)
        Razor::getInstance().get_Xfitman()->raiseWindow(client);
}

/**
 * @brief returns if the window has hiddenflag turned on
 */
bool RazorTask::isHidden()
{
    hidden = Razor::getInstance().get_Xfitman()->isHidden(client);
    return hidden;
}

/**
 * @brief returns true if task has inputfocus
 */
bool RazorTask::isActive()
{
    return (Razor::getInstance().get_Xfitman()->getActiveAppWindow()==client);
}


/**
 * @brief requests Xfitman to set the window to fullscreen (UNTESTED!)
 */
void RazorTask::setFullscreen()
{
    //first remove hidden-flag so make us visible / unminimize
    Razor::getInstance().get_Xfitman()->setClientStateFlag(client,"net_wm_state_hidden",0);
    //then add the fullscreen-flag
    Razor::getInstance().get_Xfitman()->setClientStateFlag(client,"net_wm_state_fullscreen",1);
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
bool RazorTaskManager::handleEvent(XEvent* _event)
{
    //destroy or create windows? thats whats interesting.. we dont care about the rest!
    //so we just fetch "propertynotify" events
    if (_event->type == PropertyNotify)
        updateMap();
    return false;
}

/**
 * @brief constructor
 */
RazorTaskManager::RazorTaskManager(int _bar) : RazorPlugin(_bar)
{
    qDebug() << "Razortaskmanager init";
    //now we setup our gui element
    gui = new RazorBarTask(this);
    //now we need an updated map for the clients running
    //updateMap();

    qDebug() << "Razortaskmanager updateMap done";
    //now we add our taskbar to the panel
    // it's aligned to left and it should occypy all free space
    // in the panel.
    // TODO: it doesn't work with sizeHints and with the stretch =1 too...

    Razor::getInstance().get_gui()->addWidget(gui,_bar, 1, Qt::AlignLeft);

    qDebug() << "Razortaskmanager added widget";
    //we need the events so we can process the tasks correctly

    Razor::getInstance().get_events()->registerCallback(this);

    qDebug() << "Razortaskmanager callback registered";

}
/**
 * @brief updates our clientmap
 */
void RazorTaskManager::updateMap()
{
    QList<Window>* tmp = Razor::getInstance().get_Xfitman()->getClientlist();

    //first we need to get rid of tasks that got closed
    QMapIterator<Window,RazorTask*> iter(clientList);
    while (iter.hasNext())
    {
        iter.next();
        if (!tmp->contains(iter.key()))
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
    for (int i = 0; i < tmp->count(); i++)
    {
        //add new clients to the list making new entries for the new windows
        if (!clientList.contains(tmp->at(i)) && Razor::getInstance().get_Xfitman()->acceptWindow(tmp->at(i)))
        {
            RazorTask* rtask = new RazorTask(tmp->at(i),Razor::getInstance().get_Xfitman()->getWindowDesktop(tmp->at(i)));
            qDebug() << "title: " <<rtask->getTitle();
            clientList[tmp->at(i)]=rtask;
        }
    }

    delete tmp;

    //then update the stuff in our gui
    gui->updateTasks(&clientList);
    gui->updateFocus();
}

/**
 * @brief destructor
 */
RazorTaskManager::~RazorTaskManager()
{
    for (int i = 0; i < clientList.values().count(); i++)
        delete clientList.values().at(i);
}



#endif
