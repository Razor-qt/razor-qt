#ifndef RAZORTRAY_H
#define RAZORTRAY_H

#include "razorpanelplugin.h"
#include "razorpanel.h"

#include <X11/X.h>

class XfitMan;

// the codes for the tray-operations
#define SYSTEM_TRAY_REQUEST_DOCK 0
#define SYSTEM_TRAY_BEGIN_MESSAGE 1
#define SYSTEM_TRAY_CANCEL_MESSAGE 2



/**
 * @brief This makes our trayplugin
 */

class RazorTray: public RazorPanelPlugin
{
    Q_OBJECT
public:
    RazorTray(RazorPanel* panel, const QString& configId, QWidget *parent = 0);
    ~RazorTray();

private:
    Atom m_traycode;
    XfitMan *m_xfitman;

    void swallowXEmbed(Window _wid);

private slots:
    void closeEmbed();
    void handleEvent(XEvent* event);
};

EXPORT_RAZOR_PANEL_PLUGIN_H

#endif
