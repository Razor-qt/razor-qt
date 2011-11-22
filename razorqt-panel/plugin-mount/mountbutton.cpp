/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
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

    @date   2010-11-1
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
#include <QtGui/QDesktopWidget>

#include "qtxdg/xdgicon.h"
#include "mountbutton.h"



Popup::Popup(QWidget* parent):
    QWidget(parent,  Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::X11BypassWindowManagerHint),
    mCount(0),
    mPos(0,0),
    mAnchor(Qt::TopLeftCorner)
{
    setObjectName("RazorMountPopup");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //setMinimumSize(5, 5);
    setLayout(new QGridLayout(this));
    //layout()->setMargin(0);
    layout()->setSizeConstraint(QLayout::SetFixedSize);
}


MenuDiskItem *Popup::addItem(const DiskInfo &info)
{
    MenuDiskItem  *item   = new MenuDiskItem(info, this);
    layout()->addWidget(item);
    mCount++;
    return item;
}


void Popup::deleteItem(const DiskInfo &info)
{
    MenuDiskItem* item = itemByDevice(info.device_name);
    if (item)
    {
        mCount--;
        layout()->removeWidget(item);
        item->deleteLater();
    }

    if (!mCount)
        hide();
}


MenuDiskItem *Popup::itemByDevice(const QString &deviceName)
{
    QList<MenuDiskItem*> items = this->findChildren<MenuDiskItem*>();
    foreach (MenuDiskItem* item, items)
    {
        if (item->deviceName() == deviceName)
            return item;
    }

    return 0;
}


void Popup::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    realign();
}


void Popup::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    emit visibilityChanged(true);
}


void Popup::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    emit visibilityChanged(false);
}


void Popup::realign()
{
    QRect rect;
    rect.setSize(sizeHint());
    switch (mAnchor)
    {
    case Qt::TopLeftCorner:
        rect.moveTopLeft(mPos);
        break;

    case Qt::TopRightCorner:
        rect.moveTopRight(mPos);
        break;

    case Qt::BottomLeftCorner:
        rect.moveBottomLeft(mPos);
        break;

    case Qt::BottomRightCorner:
        rect.moveBottomRight(mPos);
        break;

    }

    QRect screen = QApplication::desktop()->availableGeometry(mPos);

    if (rect.right() > screen.right())
        rect.moveRight(screen.right());

    if (rect.bottom() > screen.bottom())
        rect.moveBottom(screen.bottom());

    move(rect.topLeft());
}


void Popup::open(QPoint pos, Qt::Corner anchor)
{
    mPos = pos;
    mAnchor = anchor;
    realign();
    show();
}


MountButton::MountButton(QWidget * parent, RazorPanel *panel) :
    QToolButton(parent),
    mPopup(this),
    m_panel(panel),
    mDevAction(DevActionInfo),
    mPopupHideDelay(5000)
{
    if (!QDBusConnection::systemBus().isConnected())
    {
        qWarning() << "Can't connect to dbus daemon. Some functions will be omited";
    }

    setIcon(XdgIcon::fromTheme(QStringList() << "device-notifier" << "drive-removable-media-usb"));
    setToolTip(tr("Removable media/devices manager"));

    initialScanDevices();

    QDBusConnection conn = QDBusConnection::systemBus();
    // TODO: Check for connection, timer for reconect
    /*bool connected =*/ conn.connect("org.freedesktop.UDisks",
                                  "/org/freedesktop/UDisks",
                                  "org.freedesktop.UDisks",
                                  "DeviceChanged",
                                  this,
                                  SLOT(onDbusDeviceChangesMessage(QDBusObjectPath)));


    connect(&mPopup, SIGNAL(visibilityChanged(bool)), this, SLOT(setDown(bool)));

    connect(m_panel, SIGNAL(positionChanged()), &mPopup, SLOT(hide()));

    connect(this, SIGNAL(clicked()), this, SLOT(showHidePopup()));

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
    QList<DiskInfo*> devices = _dm.scanDevices();
    for (int i = 0; i < devices.count(); i++)
    {
        DiskInfo *disk = devices.at(i);
        // add device
        _sm.addDevice(*disk);
        addMenuItem(*disk);
    }
    qDeleteAll(devices);
}


void MountButton::addMenuItem(const DiskInfo &info)
{
    MenuDiskItem  *item   = mPopup.addItem(info);
    // Connect signals
    connect(item, SIGNAL(mountMedia(const QString&)),
            this, SLOT(onMediaMount(const QString&)));

    connect(item, SIGNAL(ejectMedia(const QString&)),
            this, SLOT(onMediaEject(const QString&)));

}


void MountButton::showMessage(const QString &text)
{
    QToolTip::showText(mapToGlobal(QPoint(0, 0)), QString("<nobr>%1</nobr>").arg(text));
}


void MountButton::showError(const QString &text)
{
    QToolTip::showText(mapToGlobal(QPoint(0, 0)), QString("<nobr><b>Error:</b><hr>%1</nobr>").arg(text), this);
}


/**************************************************************************************************/
/* Signals ---------------------------------------------------------------------------------------*/
/**************************************************************************************************/

void MountButton::onDiskAdded(DiskInfo info)
{
    if (mPopup.itemByDevice(info.device_name))
    {
        _sm.removeDevice(info);
        mPopup.deleteItem(info);
    }

    _sm.addDevice(info);
    addMenuItem(info);
    switch (mDevAction)
    {
    case DevActionInfo:
        showMessage(tr("The device <b><nobr>\"%1\"</nobr></b> is connected.").arg(info.name));
        break;

    case DevActionMenu:
        showPopup();
        mPopupHideTimer.singleShot(mPopupHideDelay, &mPopup, SLOT(hide()));
        break;

    default:
        break;
    }
}


void MountButton::onDiskRemoved(DiskInfo info)
{
    _sm.removeDevice(info);

    if (mDevAction == DevActionInfo)
        showMessage(tr("The device <b><nobr>\"%1\"</nobr></b> is removed.").arg(info.name));

    mPopup.deleteItem(info);
}


void MountButton::onDbusDeviceChangesMessage(QDBusObjectPath device)
{
    QString path = device.path();

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

    MenuDiskItem *mdi = mPopup.itemByDevice(dev_name);
    if (mdi)
    {
        mdi->setMountStatus(is_mounted);
    }

    if (item->isMounted() != old_state)
    {
        if (item->isMounted())
        {
            switch (mDevAction)
            {
            case DevActionInfo:
                showMessage(tr("The device <b><nobr>\"%1\"</nobr></b> is mounted to %2").arg(dev_name).arg(item->getMountPoint()));
                break;

            case DevActionMenu:
                showMenu();
                break;

            default:
                break;
            }
        }
        else
        {
            if (mDevAction == DevActionInfo)
                showMessage(tr("The device <b><nobr>\"%1\"</nobr></b> is unmounted<br>\nNow you can eject USB Flash or CD/DVD Disk.").arg(dev_name));
        }
    }
}


void MountButton::setDown(bool down)
{
    QToolButton::setDown(down);
}


void MountButton::onMediaMount(const QString &device)
{
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
        showError(tr("Can't mount device: %1<br>\n%2").arg(device).arg(status_text));
        return;
    }

    if (item->isMounted() != old_state)
    {
        showMessage(tr("Device '%1' is mounted to %2").arg(device).arg(mount_point));
    }

    // Run manager
    // Instead of spaces, the string contains "\040" I plan to deal with it later,
    // when I will rewrite libmount. While using this dirty hack.
    mount_point = mount_point.replace("\\040", " ", Qt::CaseSensitive);
    QDesktopServices::openUrl(QUrl::fromLocalFile(mount_point));
    mPopup.hide();
}

void MountButton::onMediaEject(const QString &device)
{
    //qDebug() << "UnMount media: " << qPrintable(device);

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
        showError(tr("Can't unmount device: %1<br>\n%2").arg(device).arg(status_text));
        return;
    }

    showMessage(tr("Device '%1' is unmounted").arg(device));
    mPopup.hide();
}


void MountButton::showHidePopup()
{
    if (mPopup.isVisible())
        mPopup.hide();
    else
    {
        mPopupHideTimer.stop();

        if (mPopup.count())
            showPopup();
        else
            showMessage(tr("No devices Available."));
    }
}


void MountButton::showPopup()
{
    if (mPopup.isVisible())
        return;

    if (!mPopup.count())
        return;

    mPopup.updateGeometry();

    QPoint p;
    if (isLeftToRight())
    {
        switch (m_panel->position())
        {
        case RazorPanel::PositionTop:
            mPopup.open(mapToGlobal(geometry().bottomLeft()), Qt::TopLeftCorner);
            break;

        case RazorPanel::PositionBottom:
            mPopup.open(mapToGlobal(geometry().topLeft()), Qt::BottomLeftCorner);
            break;

        case RazorPanel::PositionLeft:
            mPopup.open(mapToGlobal(geometry().topRight()), Qt::TopLeftCorner);
            break;

        case RazorPanel::PositionRight:
            mPopup.open(mapToGlobal(geometry().topLeft()), Qt::TopLeftCorner);
            break;
        }
    }
    else
    {
        switch (m_panel->position())
        {
        case RazorPanel::PositionTop:
            mPopup.open(mapToGlobal(geometry().bottomRight()), Qt::TopRightCorner);
            break;

        case RazorPanel::PositionBottom:
            mPopup.open(mapToGlobal(geometry().topRight()), Qt::BottomRightCorner);
            break;

        case RazorPanel::PositionLeft:
            mPopup.open(mapToGlobal(geometry().topRight()), Qt::TopLeftCorner);
            break;

        case RazorPanel::PositionRight:
            mPopup.open(mapToGlobal(geometry().topLeft()), Qt::TopLeftCorner);
            break;
        }
    }
}
