#ifndef RAZORTRAY_H
#define RAZORTRAY_H

#include "razorpanelplugin.h"
#include "razorpanel.h"

#include <X11/X.h>

// the codes for the tray-operations
#define SYSTEM_TRAY_REQUEST_DOCK 0
#define SYSTEM_TRAY_BEGIN_MESSAGE 1
#define SYSTEM_TRAY_CANCEL_MESSAGE 2

#include <QX11EmbedContainer>
#include <QFrame>


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

    void swallowXEmbed(Window _wid);

private slots:
    void closeEmbed();
    void handleEvent(XEvent* event);

    void embedError(QX11EmbedContainer::Error e);
};

EXPORT_RAZOR_PANEL_PLUGIN_H

#endif
