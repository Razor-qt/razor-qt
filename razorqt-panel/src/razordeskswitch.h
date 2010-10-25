#ifndef RAZORDESKSWITCH_H
#define RAZORDESKSWITCH_H
#include "defs.h"
#include "razorplugin.h"
/**
 * @file razordeskswitch.h
 * @author Christopher "VdoP" Regali
 * @brief Declares Razordeskswitch
 */
#include "razor.h"

/**
 * @brief a simple QSpinBox derivative for switching desktops
 */
class Razordeskswitchgui: public QSpinBox
{
    Q_OBJECT
public:
    Razordeskswitchgui(Razorplugin* _owner);
private:
    Razorplugin* owner;

signals:
    void changeDesk(int);

protected:
    void wheelEvent(QWheelEvent* _event);

};

/**
 * @brief the plugin for switching Desktops via Razorbar
 */
class Razordeskswitch:  public QObject,public Razorplugin
{
    Q_OBJECT
public:
    Razordeskswitch(int _bar);
    ~Razordeskswitch();
    virtual bool handleEvent(XEvent* _event);

private:
    int desktop;
    Razordeskswitchgui* gui;
public slots:
    void switchDesktop(int);

};



#endif
