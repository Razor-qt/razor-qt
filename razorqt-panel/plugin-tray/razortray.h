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

    int widthForHeight(int h)
    {
        int ret = layout->count() * h;
        return ret ? ret : h;
    }

signals:
    void sizeChanged();
public slots:
    void updateLayout();
private:
    QHBoxLayout* layout;
    RazorTray* owner;
private slots:
    void closeEmbed();
};

/**
 * @brief This makes our trayplugin
 */

class RazorTray: public RazorPlugin
{
    Q_OBJECT
public:
    RazorTray( RazorBar * panel, QWidget * parent, const QString & name);
    ~RazorTray();
    bool handleEvent(XEvent* _event);

    int widthForHeight(int h)
    {
        return gui->widthForHeight(h);
    }
    int heightForWidth(int w)
    {
        return w;
    }
    RazorPlugin::RazorPluginSizing sizePriority()
    {
        return RazorPlugin::Static;
    }

private:
    RazorTrayGUI* gui;
    Atom traycode;
};

extern "C" RazorPlugin* init(RazorBar* panel, QWidget* parent, const QString & name);

#endif
