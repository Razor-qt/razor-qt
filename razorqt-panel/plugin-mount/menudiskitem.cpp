/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include <QtGui/QDesktopServices>
#include "menudiskitem.h"
#include <razormount/razormount.h>
#include <qtxdg/xdgicon.h>
#include <QtCore/QDebug>
#include <QtCore/QUrl>
#include <QHBoxLayout>
#include <QToolButton>
#include <QEvent>

/************************************************

 ************************************************/
MenuDiskItem::MenuDiskItem(RazorMountDevice *device, QWidget *parent):
    QFrame(parent),
    mDevice(device)
{
    mDiskButton = new QToolButton(this);
    mDiskButton->setObjectName("DiskButton");
    mDiskButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mDiskButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    connect(mDiskButton, SIGNAL(clicked()), this, SLOT(diskButtonClicked()));

    mEjectButton =  new QToolButton(this);
    mEjectButton->setObjectName("EjectButton");
    connect(mEjectButton, SIGNAL(clicked()), this, SLOT(ejectButtonClicked()));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(mDiskButton);
    layout->addWidget(mEjectButton);
    this->setLayout(layout);

    layout->setMargin(0);
    layout->setSpacing(0);

    mEjectButton->setIcon(XdgIcon::fromTheme("media-eject"));

    connect(device, SIGNAL(destroyed()),
              this, SLOT(free()));

    connect(device, SIGNAL(changed()),
              this, SLOT(update()));

    connect(device, SIGNAL(mounted()),
              this, SLOT(mounted()));

    connect(device, SIGNAL(unmounted()),
              this, SLOT(unmounted()));

    update();
}


/************************************************

 ************************************************/
void MenuDiskItem::free()
{
    this->deleteLater();
}


/************************************************

 ************************************************/
void MenuDiskItem::update()
{
    mDiskButton->setIcon(XdgIcon::fromTheme(QStringList()
                            << mDevice->iconName()
                            << "drive-removable-media-usb"
                        ));

    QString label = mDevice->label();
    mDiskButton->setText(label);

    setMountStatus(mDevice->isMounted());
}


/************************************************

 ************************************************/
bool MenuDiskItem::isUsableDevice(const RazorMountDevice *device)
{
    switch (device->mediaType())
    {
    case RazorMountDevice::MediaTypeFdd:
        return  true;

    case RazorMountDevice::MediaTypeOptical:
        return true;
        break;

    case RazorMountDevice::MediaTypePartition:
    case RazorMountDevice::MediaTypeDrive:
        return device->isExternal();
        break;

    default:
        return false;
    }

    return false;
}


/************************************************

 ************************************************/
void MenuDiskItem::setMountStatus(bool is_mount)
{
    mEjectButton->setEnabled(is_mount);
}


/************************************************

 ************************************************/
void MenuDiskItem::diskButtonClicked()
{
    if (!mDevice->isMounted())
        mDevice->mount();
    else
        mounted();

    qobject_cast<QWidget*>(parent())->hide();
}


/************************************************

 ************************************************/
void MenuDiskItem::mounted()
{
    QDesktopServices::openUrl(QUrl(mDevice->mountPath()));
}


/************************************************

 ************************************************/
void MenuDiskItem::ejectButtonClicked()
{
    mDevice->unmount();
    setMountStatus(mDevice->isMounted());

    qobject_cast<QWidget*>(parent())->hide();
}


/************************************************

 ************************************************/
void MenuDiskItem::unmounted()
{
    if (mDevice->isEjectable())
        mDevice->eject();
}

