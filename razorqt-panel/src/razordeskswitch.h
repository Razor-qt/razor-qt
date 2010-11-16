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
class RazorDeskSwitchGUI: public QSpinBox
{
    Q_OBJECT
public:
    RazorDeskSwitchGUI(RazorPlugin* _owner);
private:
    RazorPlugin* owner;

signals:
    void changeDesk(int);

protected:
    void wheelEvent(QWheelEvent* _event);

};

/**
 * @brief the plugin for switching Desktops via Razorbar
 */
class RazorDeskSwitch:  public QObject,public RazorPlugin
{
    Q_OBJECT
public:
    RazorDeskSwitch(int _bar);
    ~RazorDeskSwitch();
    virtual bool handleEvent(XEvent* _event);

private:
    int desktop;
    RazorDeskSwitchGUI* gui;
public slots:
    void switchDesktop(int);

};



#endif
