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

QString RazorMountDevice::sizeToString(qulonglong size)
{
    double n;
    n = size / (1024.0 * 1024 * 1024);
    if (n >= 1.0)
        return QObject::tr("%1 GB").arg(n, 0, 'f', 1);

    n = size / (1024.0 * 1024);
    if (n >= 1.0)
        return QObject::tr("%1 MB").arg(n, 0, 'f', 1);

    n = size / (1024.0);
    if (n >= 1.0)
        return QObject::tr("%1 KB").arg(n, 0, 'f', 1);

    return QObject::tr("%1 B").arg(size);
}


RazorMountManager::RazorMountManager(QObject *parent):
    QObject(parent),
    mProvider(0)
{
    mProvider = new UDisks2Provider(this);
    if (!mProvider->isValid())
    {              
        delete mProvider;
        mProvider = 0;

        mProvider = new UDiskProvider(this);
        if (!mProvider->isValid())
        {
            delete mProvider;
            mProvider = 0;
        }
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
    else
        qDebug() << "RazorMountDeviceList RazorMountManager::update() no valid provider in use";

}


const RazorMountDeviceList RazorMountManager::devices() const
{
    if (mProvider)
    {
        qDebug() << "RazorMountManager::devices" << mProvider->devices();
        return mProvider->devices();
    }
    else
    {
        qDebug() << "RazorMountDeviceList RazorMountManager::devices() no valid provider in use";
        return RazorMountDeviceList();
    }
}


QDebug operator<<(QDebug dbg, const RazorMountDevice &device)
{
    dbg << device.devFile();

    switch (device.mediaType())
    {
    case RazorMountDevice::MediaTypeUnknown:    dbg<<"Type: MediaTypeUnknown";  break;
    case RazorMountDevice::MediaTypeDrive:      dbg<<"Type: MediaTypeDrive";    break;
    case RazorMountDevice::MediaTypePartition:  dbg<<"Type: MediaTypePartition";break;
    case RazorMountDevice::MediaTypeFdd:        dbg<<"Type: MediaTypeFdd";      break;
    case RazorMountDevice::MediaTypeOptical:    dbg<<"Type: MediaTypeOptical";  break;
    default:                                    dbg<<"Type: "<<device.mediaType();break;
    }
    dbg << "Label: " << device.label();
    dbg << "Mount path:" << device.mountPath();
    return dbg.space();
}


QDebug operator<<(QDebug dbg, const RazorMountDevice *device)
{
    return operator<<(dbg, *device);
}
