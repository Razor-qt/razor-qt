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
#include "mountbutton.h"
#include <razormount/razormount.h>
#include <razorqt/razornotification.h>


Popup::Popup(RazorMountManager *manager, QWidget* parent):
    QWidget(parent,  Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::X11BypassWindowManagerHint),
    mManager(manager),
    mPos(0,0),
    mAnchor(Qt::TopLeftCorner)
{
    setObjectName("RazorMountPopup");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //setMinimumSize(5, 5);
    setLayout(new QGridLayout(this));
    //layout()->setMargin(0);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    setAttribute(Qt::WA_AlwaysShowToolTips);

    connect(mManager, SIGNAL(deviceAdded(RazorMountDevice*)),
                this, SLOT(addItem(RazorMountDevice*)));

    foreach(RazorMountDevice *device, mManager->devices())
    {
        addItem(device);
    }
}


MenuDiskItem *Popup::addItem(RazorMountDevice *device)
{
    if (MenuDiskItem::isUsableDevice(device))
    {
        MenuDiskItem  *item   = new MenuDiskItem(device, this);
        layout()->addWidget(item);
        return item;
    }
    else
    {
        return 0;
    }
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
    mPopup(0),
    mPanel(panel),
    mDevAction(DevActionInfo),
    mPopupHideDelay(5000)
{
    setIcon(XdgIcon::fromTheme(QStringList() << "device-notifier" << "drive-removable-media-usb"));

    setToolTip(tr("Removable media/devices manager"));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mPopup = new Popup(&mManager, this);

    connect(mPopup, SIGNAL(visibilityChanged(bool)), this, SLOT(setDown(bool)));

    connect(mPanel, SIGNAL(positionChanged()), mPopup, SLOT(hide()));

    connect(this, SIGNAL(clicked()), this, SLOT(showHidePopup()));

    connect(&mManager, SIGNAL(deviceAdded(RazorMountDevice*)),
            this, SLOT(onDeviceAdded(RazorMountDevice*)));

    connect(&mManager, SIGNAL(deviceRemoved(RazorMountDevice*)),
            this, SLOT(onDeviceRemoved(RazorMountDevice*)));

    mManager.update();
}


MountButton::~MountButton()
{
}



void MountButton::showMessage(const QString &text)
{
//    QToolTip::showText(mapToGlobal(QPoint(0, 0)), QString("<nobr>%1</nobr>").arg(text));
    RazorNotification::notify(toolTip(), text
#if QT_VERSION >= 0x040700
                              , icon().name()
#endif
                              );
}


void MountButton::onDeviceAdded(RazorMountDevice *device)
{
    switch (mDevAction)
    {
    case DevActionInfo:
        if (MenuDiskItem::isUsableDevice(device))
            showMessage(tr("The device <b><nobr>\"%1\"</nobr></b> is connected.").arg(device->label()));
        break;

    case DevActionMenu:
        showPopup();
        mPopupHideTimer.singleShot(mPopupHideDelay, mPopup, SLOT(hide()));
        break;

    case DevActionNothing:
        break;
    }
}

void MountButton::onDeviceRemoved(RazorMountDevice *device)
{
    switch (mDevAction)
    {
    case DevActionInfo:
        if (MenuDiskItem::isUsableDevice(device))
            showMessage(tr("The device <b><nobr>\"%1\"</nobr></b> is removed.").arg(device->label()));
        break;

    case DevActionMenu:
        if (mManager.devices().isEmpty())
            hidePopup();
        break;

    case DevActionNothing:
        break;
    }
}

void MountButton::setDown(bool down)
{
    QToolButton::setDown(down);
}


void MountButton::showHidePopup()
{
    if (mPopup->isVisible())
        mPopup->hide();
    else
    {
        mPopupHideTimer.stop();

        if (mManager.devices().isEmpty())
            showMessage(tr("No devices Available."));
        else
            showPopup();
    }
}


void MountButton::showPopup()
{

    if (mPopup->isVisible())
        return;

    if (mManager.devices().isEmpty())
        return;

    mPopup->updateGeometry();

    QPoint p;
    if (isLeftToRight())
    {
        switch (mPanel->position())
        {
        case RazorPanel::PositionTop:
            mPopup->open(mapToGlobal(geometry().bottomLeft()), Qt::TopLeftCorner);
            break;

        case RazorPanel::PositionBottom:
            mPopup->open(mapToGlobal(geometry().topLeft()), Qt::BottomLeftCorner);
            break;

        case RazorPanel::PositionLeft:
            mPopup->open(mapToGlobal(geometry().topRight()), Qt::TopLeftCorner);
            break;

        case RazorPanel::PositionRight:
            mPopup->open(mapToGlobal(geometry().topLeft()), Qt::TopLeftCorner);
            break;
        }
    }
    else
    {
        switch (mPanel->position())
        {
        case RazorPanel::PositionTop:
            mPopup->open(mapToGlobal(geometry().bottomRight()), Qt::TopRightCorner);
            break;

        case RazorPanel::PositionBottom:
            mPopup->open(mapToGlobal(geometry().topRight()), Qt::BottomRightCorner);
            break;

        case RazorPanel::PositionLeft:
            mPopup->open(mapToGlobal(geometry().topRight()), Qt::TopLeftCorner);
            break;

        case RazorPanel::PositionRight:
            mPopup->open(mapToGlobal(geometry().topLeft()), Qt::TopLeftCorner);
            break;
        }
    }
}


void MountButton::hidePopup()
{
    mPopup->hide();
}
