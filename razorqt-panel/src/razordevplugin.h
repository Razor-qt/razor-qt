#ifndef RAZORDEVPLUGIN_H
#define RAZORDEVPLUGIN_H
#include "defs.h"
#include "razorplugin.h"
#include <razorqt/razordevman.h>
#include <razorqt/razorhal.h>

class RazorDevicePluginGui;
class RazorDevicePlugin :public QObject, public Razorplugin
{
  Q_OBJECT
public:
    RazorDevicePlugin(int _bar);
    Razordevman* backEnd() { return deviceBackend; }
private:
  Razordevman* deviceBackend;
  RazorDevicePluginGui* gui;
private slots:
  void deviceAdded(QString);
  void deviceRemoved(QString);
  
public slots:
  void deviceMount(QAction* _action);
  
signals:
  void deviceMounted(QString);
  void deviceUMounted(QString);
};


class RazorDevicePluginGui : public QToolButton
{
  Q_OBJECT
public:
    RazorDevicePluginGui(RazorDevicePlugin* _owner);
    ~RazorDevicePluginGui();
  void addDevice(QString);
  void removeDevice(QString);
private:
  RazorDevicePlugin* owner;
  QMap<QString,QAction*> deviceList;
  QMenu* deviceMenu;
};



#endif
