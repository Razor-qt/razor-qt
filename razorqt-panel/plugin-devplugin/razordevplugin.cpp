#ifndef RAZORDEVPLUGIN_CPP
#define RAZORDEVPLUGIN_CPP
#include "razordevplugin.h"
#include "razor.h"

RazorPlugin* init(RazorBar* panel, QWidget* parent, const QString & name)
{
    RazorDevicePlugin * ret = new RazorDevicePlugin(panel, parent, name);
    Q_ASSERT(ret);
    return ret;
}

void RazorDevicePlugin::deviceAdded(QString _uuid)
{
    qDebug() << "RazorDevicePlugin: add triggered!";
    gui->addDevice(_uuid);
}

void RazorDevicePlugin::deviceMount(QAction* _action)
{
    QString uuid=_action->data().toString();
    QMessageBox msg_box;

    qDebug() << "RazorDevicePlugin: u/mount triggered! uuid: "<<_action->data();

    if (deviceBackend->getDevice(uuid)->getMounted())
    {
        if (deviceBackend->umount(uuid))
        {
            msg_box.setText("Device unmounted!");
        }
        else
        {
            msg_box.setText("unmount failed");
        }
        msg_box.exec();
    }
    else if (!deviceBackend->getDevice(uuid)->getMounted())
    {
        if (deviceBackend->mount(uuid))
        {
            msg_box.setText("Device mounted! ("+deviceBackend->getDevice(uuid)->getMountPoint()+")");
        }
        else
        {
            msg_box.setText("mount failed");
        }
    }

    msg_box.exec();


}

void RazorDevicePlugin::deviceRemoved(QString _uuid)
{
    qDebug() << "RazorDevicePlugin: remove triggered!";
    gui->addDevice(_uuid);
}

RazorDevicePlugin::RazorDevicePlugin(RazorBar * panel, QWidget * parent, const QString & name): RazorPluginSquare(panel, parent, name)
{

    //first select the backend, since we only have HAL for now - we use hal in any situation
    deviceBackend = new RazorHal();


    //now we connect it to the right signals
    connect (deviceBackend, SIGNAL(deviceAdded(QString)), this, SLOT(deviceAdded(QString)));
    connect (deviceBackend, SIGNAL(deviceRemoved(QString)), this, SLOT(deviceAdded(QString)));


    gui = new RazorDevicePluginGUI(this);
    mainLayout()->addWidget(gui);
    //gui->setFixedHeight(Razor::getInstance().get_looknfeel()->getInt("razorbar_height")-6);
    //gui->setFixedWidth(Razor::getInstance().get_looknfeel()->getInt("razorbar_height")-6);
    //Razor::getInstance().get_gui()->addWidget(gui,_bar,0,Qt::AlignLeft);


}







void RazorDevicePluginGUI::addDevice(QString _uuid)
{
    if (!deviceList.contains(_uuid))
    {
        QAction* tmp = new QAction((QIcon) "", owner->backEnd()->getDevice(_uuid)->getvolume()+"  ("+ owner->backEnd()->getDevice(_uuid)->getblockDev()+")", deviceMenu);
        tmp->setData(_uuid);
        deviceMenu->addAction(tmp);
        deviceList[_uuid]=tmp;
    }
}

//TODO we need to setup some icons for this class :)
// like device, device mounted, device unmounted and stuff

RazorDevicePluginGUI::RazorDevicePluginGUI(RazorDevicePlugin* parent): QToolButton(NULL)
{
    owner = parent;

    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setPopupMode(QToolButton::InstantPopup);




    deviceMenu = new QMenu(this);
    setMenu(deviceMenu);
    connect (this, SIGNAL(triggered(QAction*)), owner, SLOT(deviceMount(QAction*)));
    show();

}


void RazorDevicePluginGUI::removeDevice(QString _uuid)
{
    if (deviceList.contains(_uuid))
    {
        deviceMenu->removeAction(deviceList.value(_uuid));
        delete deviceList.value(_uuid);
        deviceList.remove(_uuid);
    }
}

RazorDevicePluginGUI::~RazorDevicePluginGUI()
{
    delete deviceMenu;
}





#endif
