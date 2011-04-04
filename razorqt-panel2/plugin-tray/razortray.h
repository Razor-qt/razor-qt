#ifndef RAZORTRAY_H
#define RAZORTRAY_H

#include "../panel/razorpanelplugin.h"
#include "../panel/razorpanel.h"

#include <X11/X.h>

// the codes for the tray-operations
#define SYSTEM_TRAY_REQUEST_DOCK 0
#define SYSTEM_TRAY_BEGIN_MESSAGE 1
#define SYSTEM_TRAY_CANCEL_MESSAGE 2

#define _NET_SYSTEM_TRAY_ORIENTATION_HORZ 0
#define _NET_SYSTEM_TRAY_ORIENTATION_VERT 1

#include <QX11EmbedContainer>


/**
 * @brief This makes our trayplugin
 */

class RazorTray: public RazorPanelPlugin
{
    Q_OBJECT
public:
    RazorTray(const RazorPalelPluginStartInfo* startInfo, QWidget* parent = 0);
    ~RazorTray();

    virtual Alignment preferredAlignment() const { return AlignRight; }

private:
    Atom m_traycode;
    int m_count;

    void swallowXEmbed(Window _wid);
    void updateSize();

private slots:
    void closeEmbed();
    void handleEvent(XEvent* event);

    void embedError(QX11EmbedContainer::Error e);
};

EXPORT_RAZOR_PANEL_PLUGIN_H

#endif
