/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)GPL2
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

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QProcess>

#include "storageitem.h"
#include "mount.h"

StorageItem::StorageItem(DiskInfo info, QObject *parent) :
    QObject(parent)
{
    _udev_info     = info;
    _mount_options = "defaults";
    _is_mounted    = false;

    QStringList mount = ::isMounted(info.device_name, MC_DEVICE);

    // Если примонтирован уже, то точку монтирования назначить на указанную
    if (mount.count() > 0)
    {
        _is_mounted    = true;
        _mount_point = mount[1];
    }
}

StorageItem::~StorageItem()
{
}

QString StorageItem::getMountPoint()
{
    return _mount_point;
}

bool StorageItem::isMounted()
{
    return _is_mounted;
}

bool StorageItem::mount(QString &text_status)
{
    bool status = ::diskMount(UDISKS, _udev_info.device_name, _mount_point, text_status);
    if (status)
    {
        _is_mounted = true;
        if (_mount_point.isEmpty())
        {
            QStringList mounts = ::isMounted(_udev_info.device_name, MC_DEVICE);
            if (mounts.count() > 0)
            {
                _mount_point = mounts.at(1);
            }
            else
            {
                _is_mounted = false;
                status      = false;
            }
        }
    }

    return status;
}

bool StorageItem::unmount(QString &text_status)
{
    bool status = ::diskUnMount(UDISKS, _udev_info.device_name, text_status);
    if (status)
    {
        _is_mounted  = false;
        _mount_point = QString();
    }

    return status;
}

void StorageItem::setMountStatus(bool is_mounted, const QString &mount_point)
{
    _is_mounted = is_mounted;

    if (is_mounted)
    {
        if (mount_point.isEmpty())
        {
            QStringList mounts = ::isMounted(_udev_info.device_name, MC_DEVICE);
            if (mounts.count() > 0)
            {
                _mount_point = mounts.at(1);
            }
            else
            {
                // Ибо нефиг говорить, что примонтированы, когда это не так
                _is_mounted = false;
            }
        }
        else
        {
            _mount_point = mount_point;
        }
    }
    else
    {
        _mount_point = QString();
    }
}

