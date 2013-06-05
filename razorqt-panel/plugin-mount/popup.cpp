/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011-2013 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include <QGridLayout>
#include <QLabel>
#include <QtGui/QDesktopWidget>
#include <razormount/razormount.h>
#include "menudiskitem.h"
#include "popup.h"


Popup::Popup(RazorMountManager *manager, IRazorPanelPlugin *plugin, QWidget* parent):
    QDialog(parent,  Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::X11BypassWindowManagerHint),
    mManager(manager),
    mPlugin(plugin),
    mDisplayCount(0)
{

    setObjectName("RazorMountPopup");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setLayout(new QVBoxLayout(this));
    layout()->setMargin(0);

    setAttribute(Qt::WA_AlwaysShowToolTips);

    connect(mManager, SIGNAL(deviceAdded(RazorMountDevice*)),
                this, SLOT(addItem(RazorMountDevice*)));
    connect(mManager, SIGNAL(deviceRemoved(RazorMountDevice*)),
                this, SLOT(removeItem(RazorMountDevice*)));

    mPlaceholder = new QLabel(tr("No devices are available"), this);
    mPlaceholder->setObjectName("NoDiskLabel");
    layout()->addWidget(mPlaceholder);
    mPlaceholder->hide();

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
        item->setVisible(true);
        mDisplayCount++;
        if (mDisplayCount != 0)
            mPlaceholder->hide();

        if (isVisible())
            realign();
        return item;
    }
    else
    {
        return 0;
    }
}

void Popup::removeItem(RazorMountDevice *device)
{
    if (MenuDiskItem::isUsableDevice(device))
    {
        mDisplayCount--;
        if (mDisplayCount == 0)
            mPlaceholder->show();
    }
}


void Popup::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    realign();
}


void Popup::showEvent(QShowEvent *event)
{
    if (mDisplayCount == 0)
        mPlaceholder->show();

    realign();

    QWidget::showEvent(event);
    emit visibilityChanged(true);
}


void Popup::hideEvent(QHideEvent *event)
{
    mPlaceholder->hide();

    QWidget::hideEvent(event);
    emit visibilityChanged(false);
}


void Popup::realign()
{
    updateGeometry();
    adjustSize();

    QRect rect = mPlugin->calculatePopupWindowPos(size());
    setGeometry(rect);
}


void Popup::showHide()
{
    setHidden(!isHidden());
}

