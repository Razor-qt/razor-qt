#ifndef RAZORTRAY_H
#define RAZORTRAY_H
#include "defs.h"

// the codes for the tray-operations
#define SYSTEM_TRAY_REQUEST_DOCK 0
#define SYSTEM_TRAY_BEGIN_MESSAGE 1
#define SYSTEM_TRAY_CANCEL_MESSAGE 2


/**
 * @file razortray.h
 * @author Christopher "VdoP" Regali
 * @brief declares Razortray - the Razorbar Tray-Plugin and Razortraygui - the gui for the plugin
 *
 */
#include "razorplugin.h"


class RazorTray;

/**
 * @brief manages the display-stuff of the traybar
 */

class RazorTrayGUI : public QLabel
{
    Q_OBJECT
public:
    RazorTrayGUI(RazorTray* _owner);
    ~RazorTrayGUI();
    void swallowXEmbed(Window _wid);
    void updateLayout();
private:
    QHBoxLayout* layout;
    RazorTray* owner;
};

/**
 * @brief This makes our trayplugin
 */

class RazorTray: public RazorPlugin
{
public:
    RazorTray(int _bar);
    ~RazorTray();
    virtual bool handleEvent(XEvent* _event);
private:
    RazorTrayGUI* gui;
    Atom traycode;
};

#endif
