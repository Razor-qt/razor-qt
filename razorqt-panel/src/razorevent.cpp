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
Razorevent::Razorevent(int& _argc, char** _argv): QApplication(_argc, _argv)
{

}


/**
 * @brief the destructor
 */

Razorevent::~Razorevent()
{

}

/**
 * @brief hands over the events to the plugin-handlers
 */
bool Razorevent::x11EventFilter(XEvent* _event)
{
    //qDebug() << _event->type;
    QListIterator<Razorplugin*> iter(callbackList);
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
void Razorevent::registerCallback(Razorplugin* _callbackClass)
{
    callbackList.append(_callbackClass);
}

/**
 * @brief unregisters a callback that was previously registered
 */

void Razorevent::unregisterCallback(Razorplugin* _callbackClass)
{
    if (callbackList.contains(_callbackClass))
        callbackList.removeAll(_callbackClass);
}




#endif
