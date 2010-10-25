#ifndef RAZORCLOCK_H
#define RAZORCLOCK_H
#include "defs.h"
#include "razorplugin.h"
/**
 * @file razorclock.h
 * @brief declares Razorclock and Razorclockgui
 * @author Christopher "VdoP" Regali
 */

class Razorclock;
/**
 * @brief a simple QLabel-derivative for displaying the time
 */
class Razorclockgui : public QLabel
{
public:
  Razorclockgui (Razorclock* _owner);

private:
  Razorclock* owner;
};

/**
 * @brief the clock-plugin for razorbar
 */
class Razorclock : public QObject ,public Razorplugin
{
  Q_OBJECT
public:
    Razorclock(int _bar);
    ~Razorclock();
    virtual bool handleEvent(XEvent* _event);

private:
  QTimer* clocktimer;
  Razorclockgui* gui;
  
public slots:
  void updateTime();
};




#endif
