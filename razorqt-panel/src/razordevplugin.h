#ifndef RAZORDEVPLUGIN_H
#define RAZORDEVPLUGIN_H
#include "defs.h"
#include "razorplugin.h"
#include <razorqt/razordevman.h>
#include <razorqt/razorhal.h>

class RazorDevicePluginGUI;

class RazorDevicePlugin : public RazorPluginSquare
{
    Q_OBJECT
public:
    RazorDevicePlugin(RazorBar * panel, QWidget * parent);
    RazorDevMan* backEnd()
    {
        return deviceBackend;
    }
private:
    RazorDevMan* deviceBackend;
    RazorDevicePluginGUI* gui;
private slots:
    void deviceAdded(QString);
    void deviceRemoved(QString);

public slots:
    void deviceMount(QAction* _action);

signals:
    void deviceMounted(QString);
    void deviceUMounted(QString);
};


class RazorDevicePluginGUI : public QToolButton
{
    Q_OBJECT
public:
    RazorDevicePluginGUI(RazorDevicePlugin* parent);
    ~RazorDevicePluginGUI();
    void addDevice(QString);
    void removeDevice(QString);
private:
    RazorDevicePlugin* owner;
    QMap<QString,QAction*> deviceList;
    QMenu* deviceMenu;
};



#endif
