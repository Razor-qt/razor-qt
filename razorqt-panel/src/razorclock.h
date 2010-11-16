#ifndef RAZORCLOCK_H
#define RAZORCLOCK_H
#include "defs.h"
#include "razorplugin.h"
/**
 * @file razorclock.h
 * @brief declares Razorclock and Razorclockgui
 * @author Christopher "VdoP" Regali
 */

class RazorClock;
/**
 * @brief a simple QLabel-derivative for displaying the time
 */
class RazorClockGUI : public QLabel
{
    Q_OBJECT
public:
    RazorClockGUI (RazorClock* _owner);

private:
    RazorClock* owner;
};

/**
 * @brief the clock-plugin for razorbar
 */
class RazorClock : public QObject ,public RazorPlugin
{
    Q_OBJECT
public:
    RazorClock(int _bar);
    ~RazorClock();
    virtual bool handleEvent(XEvent* _event);

private:
    QTimer* clocktimer;
    RazorClockGUI* gui;

public slots:
    void updateTime();
};




#endif
