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


class Razortray;

/**
 * @brief manages the display-stuff of the traybar
 */

class Razortraygui : public QLabel
{
public:
  Razortraygui(Razortray* _owner);
  ~Razortraygui();
  void swallowXEmbed(Window _wid);
  void updateLayout();
private:
  QHBoxLayout* layout;
  Razortray* owner;
};

/**
 * @brief This makes our trayplugin
 */

class Razortray: public Razorplugin
{
public:
    Razortray(int _bar);
    ~Razortray();
    virtual bool handleEvent(XEvent* _event);
private:
    Razortraygui* gui;
    Atom traycode;
};

#endif
