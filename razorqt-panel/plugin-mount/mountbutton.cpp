/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include <QtDebug>

#include <QtGui/QMessageBox>
#include <QtGui/QDesktopServices>
#include <QtCore/QUrl>
#include <QtGui/QToolTip>
#include <QtGui/QDesktopWidget>

#include <qtxdg/xdgicon.h>
#include <razormount/udisksmanager.h>

#include "mountbutton.h"


Popup::Popup(QWidget* parent):
    QWidget(parent,  Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::X11BypassWindowManagerHint),
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


MenuDiskItem *Popup::addItem(UdisksInfo *info)
{
    MenuDiskItem  *item   = new MenuDiskItem(info, this);
    layout()->addWidget(item);
    m_items[info] = item;
    return item;
}


void Popup::deleteItem(UdisksInfo *info)
{
    MenuDiskItem* item = m_items.take(info);
    if (item)
    {
        layout()->removeWidget(item);
        item->deleteLater();
    }

    if (!count())
        hide();
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
    
    m_manager = new UdisksManager(this);

    setIcon(XdgIcon::fromTheme(QStringList() << "device-notifier" << "drive-removable-media-usb"));
    setToolTip(tr("Removable media/devices manager"));

    initialScanDevices();

    connect(&mPopup, SIGNAL(visibilityChanged(bool)), this, SLOT(setDown(bool)));

    connect(m_panel, SIGNAL(positionChanged()), &mPopup, SLOT(hide()));

    connect(this, SIGNAL(clicked()), this, SLOT(showHidePopup()));

    connect(m_manager, SIGNAL(addDevice(UdisksInfo*)),
            this, SLOT(onDiskAdded(UdisksInfo*)));
    connect(m_manager, SIGNAL(removeDevice(UdisksInfo*)),
            this, SLOT(onDiskRemoved(UdisksInfo*)));
}


MountButton::~MountButton()
{
}


void MountButton::initialScanDevices()
{
    foreach (UdisksInfo *i, m_manager->devices())
    {
        addMenuItem(i);
    }
}


void MountButton::addMenuItem(UdisksInfo *info)
{
    mPopup.addItem(info);
}


void MountButton::showMessage(const QString &text)
{
    QToolTip::showText(mapToGlobal(QPoint(0, 0)), QString("<nobr>%1</nobr>").arg(text));
}


void MountButton::showError(const QString &text)
{
    qWarning() << "MountButton::showError" << text;
    QToolTip::showText(mapToGlobal(QPoint(0, 0)), QString("<nobr><b>Error:</b><hr>%1</nobr>").arg(text), this);
}


void MountButton::onDiskAdded(UdisksInfo *info)
{
    MenuDiskItem *item = mPopup.addItem(info);
    connect(item, SIGNAL(error(QString)), this, SLOT(showError(QString)));

    switch (mDevAction)
    {
    case DevActionInfo:
        showMessage(tr("The device <b><nobr>\"%1\"</nobr></b> is connected.").arg(info->displayName()));
        break;

    case DevActionMenu:
        showPopup();
        mPopupHideTimer.singleShot(mPopupHideDelay, &mPopup, SLOT(hide()));
        break;

    default:
        break;
    }
}


void MountButton::onDiskRemoved(UdisksInfo *info)
{
    if (mDevAction == DevActionInfo)
        showMessage(tr("The device <b><nobr>\"%1\"</nobr></b> is removed.").arg(info->displayName()));

    mPopup.deleteItem(info);
}

void MountButton::setDown(bool down)
{
    QToolButton::setDown(down);
}


//void MountButton::onMediaMount(UdisksInfo *item)
//{
//    bool    old_state   = item->isMounted();
//    QString status_text;
//    if (!item->isMounted())
//    {
//        item->mount(status_text);
//    }
//
//    QString mount_point = item->getMountPoint();
//
//    if (!item->isMounted())
//    {
        // Error
//        showError(tr("Can't mount device: %1<br>\n%2").arg(device).arg(status_text));
//        return;
//    }
//
//    if (item->isMounted() != old_state)
//    {
//        showMessage(tr("Device '%1' is mounted to %2").arg(device).arg(mount_point));
//    }
//
    // Run manager
    // Instead of spaces, the string contains "\040" I plan to deal with it later,
    // when I will rewrite libmount. While using this dirty hack.
//    mount_point = mount_point.replace("\\040", " ", Qt::CaseSensitive);
//    QDesktopServices::openUrl(QUrl::fromLocalFile(mount_point));
//    mPopup.hide();
//}
//
//void MountButton::onMediaEject(const QString &device)
//{
    //qDebug() << "UnMount media: " << qPrintable(device);
//
//    StorageItem *item = _sm.getDevice(device);
//    if (item == NULL)
//    {
//        return;
//    }
//
//    QString status_text;
//    if (item->isMounted())
//    {
//        item->unmount(status_text);
//    }
//
//    if (item->isMounted())
//    {
        // Error
//        showError(tr("Can't unmount device: %1<br>\n%2").arg(device).arg(status_text));
//        return;
//    }
//
//    showMessage(tr("Device '%1' is unmounted").arg(device));
//    mPopup.hide();
//}


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
