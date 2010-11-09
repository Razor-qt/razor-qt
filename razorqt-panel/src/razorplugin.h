#ifndef RAZORPLUGIN_H
#define RAZORPLUGIN_H

#include "defs.h"

/**
 * @file razorplugin.h
 * @brief declares Razorplugin
 * @author Christopher "VdoP" Regali
 */

/**
 * @brief declares Razorplugin, a baseclass for all the plugins
 */

class Razorplugin
{
public:
    Razorplugin(int _bar)
    {
        Q_UNUSED(_bar);
    };
    /**
     * @brief this is called by Razorevent when x11 events get in and the derived class is registered!
     */
    virtual bool handleEvent(XEvent* _event)
    {
        Q_UNUSED(_event);
        return false;
    }
private:
    QWidget* gui;

};

#endif
