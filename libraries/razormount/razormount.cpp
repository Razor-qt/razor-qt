/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
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

#include "razormount.h"
#include "rzmountproviders.h"
#include <QtCore/QDebug>

RazorMountDevice::RazorMountDevice():
    QObject(0),
    mIsValid(false),
    mIsExternal(false),
    mIsMounted(false),
    mIsEjectable(false)
{
}


RazorMountManager::RazorMountManager(QObject *parent):
    QObject(parent),
    mProvider(0)
{
    mProvider = new UDiskProvider(this);
    if (!mProvider->isValid())
    {
        delete mProvider;
        mProvider = 0;
    }

    if (!mProvider)
        return;


    update();

    connect(mProvider, SIGNAL(deviceAdded(RazorMountDevice*)),
                 this, SIGNAL(deviceAdded(RazorMountDevice*)));

    connect(mProvider, SIGNAL(deviceChanged(RazorMountDevice*)),
                 this, SIGNAL(deviceChanged(RazorMountDevice*)));

    connect(mProvider, SIGNAL(deviceRemoved(RazorMountDevice*)),
                 this, SIGNAL(deviceRemoved(RazorMountDevice*)));
}


RazorMountManager::~RazorMountManager()
{
    delete mProvider;
}


void RazorMountManager::update()
{
    if (mProvider)
        mProvider->update();
}


const RazorMountDeviceList *RazorMountManager::devices() const
{
    if (mProvider)
        return mProvider->devices();
    else
        return 0;
}


