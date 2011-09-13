/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * END_COMMON_COPYRIGHT_HEADER */
/* Based on a "MountTray" project - modified for Razor needs
    http://hatred.homelinux.net

    @date   2010-11-11
    @brief  Main application class: integrate all components

    Copyright (C) 2010 by hatred <hatred@inbox.ru>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the version 2 of GNU General Public License as
    published by the Free Software Foundation.

    For more information see LICENSE and LICENSE.ru files
*/

#include <QtDebug>

#include <QtGui/QMessageBox>
#include <QtGui/QDesktopServices>
#include <QtCore/QUrl>
#include <QtGui/QToolTip>

#include "qtxdg/xdgicon.h"
#include "mountbutton.h"


MountButton::MountButton(QWidget * parent, RazorPanel *panel)
    : QToolButton(parent),
      m_panel(panel),
      mInitialized(false)
{
    if (!QDBusConnection::systemBus().isConnected())
    {
        qDebug() << "Can't connect to dbus daemon. Some functions will be omited";
    }

    setIcon(XdgIcon::fromTheme("drive-removable-media-usb"));
    setToolTip(tr("Removable media/devices manager"));

    // Display disk menu
    m_menu = new QMenu(this);
    QAction *empty_action = m_menu->addAction(tr("Empty"));
    empty_action->setEnabled(false);
    
//    setMenu(m_menu);

    QDBusConnection conn = QDBusConnection::systemBus();
    // TODO: Check for connection, timer for reconect
    /*bool connected =*/ conn.connect("org.freedesktop.UDisks",
                                  "/org/freedesktop/UDisks",
                                  "org.freedesktop.UDisks",
                                  "DeviceChanged",
                                  this,
                                  SLOT(onDbusDeviceChangesMessage(QDBusObjectPath)));

    connect(this, SIGNAL(clicked()), this, SLOT(showMenu()));

    connect(&_dm, SIGNAL(deviceConnected(DiskInfo)),
            this, SLOT(onDiskAdded(DiskInfo)));
    connect(&_dm, SIGNAL(deviceDisconnected(DiskInfo)),
            this, SLOT(onDiskRemoved(DiskInfo)));
    _dm.start();
}

MountButton::~MountButton()
{
    _dm.exit(0);
    _dm.wait(1000);
    _dm.terminate();
    _dm.wait();
}

void MountButton::initialScanDevices()
{
    mInitialized = true;
    QList<DiskInfo*> devices = _dm.scanDevices();
    for (int i = 0; i < devices.count(); i++)
    {
        DiskInfo *disk = devices.at(i);
        // add device
        _sm.addDevice(*disk);
        addMenuItem(*disk);

        StorageItem *sitem = _sm.getDevice(*disk);
        updateMenuItem(disk->device_name, disk->file_system_label, sitem->isMounted());

        // clear
        delete disk;
    }
}

void MountButton::addMenuItem(const DiskInfo &info)
{
    MenuDiskItem  *item   = new MenuDiskItem(info, this);
    QWidgetAction *action = new QWidgetAction(this);
    action->setDefaultWidget(item);

    if (m_menu_items.count() == 0)
    {
        // Clear 'Empty' item
        m_menu->clear();
        m_menu->addAction(action);
    }
    else
    {
        // Insert on Top
        m_menu->insertAction(m_menu->actions().at(0), action);
    }

    m_menu_items.insert(info.device_name, action);

    // Connect signals
    connect(item, SIGNAL(mountMedia(const QString&)),
            this, SLOT(onMediaMount(const QString&)));

    connect(item, SIGNAL(ejectMedia(const QString&)),
            this, SLOT(onMediaEject(const QString&)));
}

void MountButton::removeMenuItem(const QString &device)
{
    QWidgetAction *action = 0;
    if (m_menu_items.contains(device))
    {
        action = m_menu_items[device];
        m_menu->removeAction(action);
        m_menu_items.remove(device);
        delete action;
    }

    if (m_menu_items.count() == 0)
    {
        QAction *empty_action = m_menu->addAction(tr("Empty"));
        empty_action->setEnabled(false);
    }
}

void MountButton::updateMenuItem(const QString &device, const QString &name, bool is_mounted)
{
    QWidgetAction *action = 0;
    if (m_menu_items.contains(device))
    {
        action = m_menu_items[device];
        MenuDiskItem *item = static_cast<MenuDiskItem*>(action->defaultWidget());

        if (item == 0)
        {
            return;
        }

        if (!name.isEmpty())
        {
            item->setLabel(name);
        }

        item->setMountStatus(is_mounted);
    }
}

void MountButton::showMessage(const QString &text)
{
    QToolTip::showText(mapToGlobal(QPoint(0, 0)), text);
}

void MountButton::showError(const QString &text)
{
    QToolTip::showText(mapToGlobal(QPoint(0, 0)), QString("<b>Error:</b><hr>%1").arg(text), this);
}

/**************************************************************************************************/
/* Signals ---------------------------------------------------------------------------------------*/
/**************************************************************************************************/

void MountButton::onDiskAdded(DiskInfo info)
{
    _sm.addDevice(info);
    addMenuItem(info);
    showMessage(tr("Device connected:<br> <b><nobr>%1</nobr></b>").arg(info.name));
}

void MountButton::onDiskRemoved(DiskInfo info)
{
    _sm.removeDevice(info);
    removeMenuItem(info.device_name);
    showMessage(tr("Device removed:<br> <b><nobr>%1</nobr></b>").arg(info.name));
}

void MountButton::onDbusDeviceChangesMessage(QDBusObjectPath device)
{
    QString path = device.path();
    qDebug() << "Changed: " << qPrintable(path);

    QDBusInterface iface("org.freedesktop.UDisks",
                         path,
                         "org.freedesktop.UDisks.Device",
                         QDBusConnection::systemBus());

    //qDebug() << "Device name: " << iface.property("DeviceFile");
    //qDebug() << "Is mounted:  " << iface.property("DeviceIsMounted");

    QString dev_name   = iface.property("DeviceFile").toString();
    bool    is_mounted = iface.property("DeviceIsMounted").toBool();

    StorageItem *item = _sm.getDevice(dev_name);
    if (item == NULL)
    {
        return;
    }

    bool old_state = item->isMounted();
    item->setMountStatus(is_mounted);
    updateMenuItem(dev_name, QString(), item->isMounted());

    if (item->isMounted() != old_state)
    {
        if (item->isMounted())
        {
            QString mount_point = item->getMountPoint();
            showMessage(tr("Device '%1' is mounted to %2").arg(dev_name).arg(mount_point));
        }
        else
        {
            showMessage(tr("Device '%1' is unmounted\nNow you can eject USB Flash or CD/DVD Disk").arg(dev_name));
        }
    }
}

void MountButton::onMediaMount(const QString &device)
{
    qDebug() << "Mount media: " << qPrintable(device) << "\n";
    m_menu->hide();

    StorageItem *item = _sm.getDevice(device);
    if (item == NULL)
    {
        return;
    }

    bool    old_state   = item->isMounted();
    QString status_text;
    if (!item->isMounted())
    {
        item->mount(status_text);
    }

    QString mount_point = item->getMountPoint();

    if (!item->isMounted())
    {
        // Error
        showError(tr("Can't mount device: %1\n%2").arg(device).arg(status_text));
        return;
    }

    if (item->isMounted() != old_state)
    {
        showMessage(tr("Device '%1' is mounted to %2").arg(device).arg(mount_point));
        updateMenuItem(device, "", true);
    }

    // Run manager
    QDesktopServices::openUrl(QUrl(mount_point));

}

void MountButton::onMediaEject(const QString &device)
{
    qDebug() << "UnMount media: " << qPrintable(device) << "\n";
    m_menu->hide();

    StorageItem *item = _sm.getDevice(device);
    if (item == NULL)
    {
        return;
    }

    QString status_text;
    if (item->isMounted())
    {
        item->unmount(status_text);
    }

    if (item->isMounted())
    {
        // Error
        showError(tr("Can't unmount device: %1\n%2").arg(device).arg(status_text));
        return;
    }

    showMessage(tr("Device '%1' is unmounted").arg(device));
    updateMenuItem(device, "", false);
}

void MountButton::showMenu()
{
    // Scan already connected devices
    if (!mInitialized)
        initialScanDevices();

    // TODO/FIXME: shared code with MainMenu plugin. How to share? Something like "menubutton base class"?
    int x, y;

    switch (m_panel->position())
    {
        case RazorPanel::PositionTop:
            x = mapToGlobal(QPoint(0, 0)).x();
            y = m_panel->mapToGlobal(QPoint(0, m_panel->sizeHint().height())).y();
            break;

        case RazorPanel::PositionBottom:
            x = mapToGlobal(QPoint(0, 0)).x();
            y = m_panel->mapToGlobal(QPoint(0, 0)).y() - m_menu->sizeHint().height();
            break;

        case RazorPanel::PositionLeft:
            x = m_panel->mapToGlobal(QPoint(m_panel->sizeHint().width(), 0)).x();
            y = mapToGlobal(QPoint(0, 0)).y();
            break;

        case RazorPanel::PositionRight:
            x = m_panel->mapToGlobal(QPoint(0, 0)).x() - m_menu->sizeHint().width();
            y = mapToGlobal(QPoint(0, 0)).y();
            break;

    }

    QPoint pos(x, y);
    m_menu->exec(pos);
}
