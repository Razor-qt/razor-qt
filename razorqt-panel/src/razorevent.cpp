#ifndef RAZOREVENT_CPP
#define RAZOREVENT_CPP

#include "razorevent.h"

/**
 * @file razorevent.cpp
 * @brief implements Razorevent
 * @author Christopher "VdoP" Regali
 */

/**
 * @brief the constructor
 */
RazorEvent::RazorEvent(int& _argc, char** _argv): QApplication(_argc, _argv)
{

}


/**
 * @brief the destructor
 */

RazorEvent::~RazorEvent()
{

}

/**
 * @brief hands over the events to the plugin-handlers
 */
bool RazorEvent::x11EventFilter(XEvent* _event)
{
    //qDebug() << _event->type;
    QListIterator<RazorPlugin*> iter(callbackList);
    while (iter.hasNext())
    {
        if (iter.next()->handleEvent(_event))
            return true;
    }
    return false;
}




/**
 * @brief registers a new callback to where the events are forwarded
 */
void RazorEvent::registerCallback(RazorPlugin* _callbackClass)
{
    callbackList.append(_callbackClass);
}

/**
 * @brief unregisters a callback that was previously registered
 */

void RazorEvent::unregisterCallback(RazorPlugin* _callbackClass)
{
    if (callbackList.contains(_callbackClass))
        callbackList.removeAll(_callbackClass);
}




#endif
