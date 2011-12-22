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

#include "storagemanager.h"

StorageManager::StorageManager(QObject *parent) :
    QObject(parent)
{
}

StorageManager::~StorageManager()
{
    qDeleteAll(_storage_items);
}

void StorageManager::addDevice(const DiskInfo &info)
{
    StorageItem *item = new StorageItem(info);
    if (item == NULL)
        return;

    // TODO: additional settings
    QString name = info.device_name;

    _storage_items.insert(name, item);
}

void StorageManager::removeDevice(const DiskInfo &info)
{
    QString name = info.device_name;
    removeDevice(name);
}

void StorageManager::removeDevice(const QString &dev_name)
{
    StorageItem *item = getDevice(dev_name);

    if (item == NULL)
        return;

    _storage_items.remove(dev_name);

    // All needed operations like unmount should be made in dtor
    delete item;
}

QList<StorageItem *> StorageManager::getDevices()
{
    return _storage_items.values();
}

StorageItem * StorageManager::getDevice(const QString &dev_name)
{
    StorageItem *item = 0;

    if (_storage_items.contains(dev_name))
    {
        item = _storage_items[dev_name];
    }

    return item;
}

StorageItem * StorageManager::getDevice(const DiskInfo &info)
{
    return getDevice(info.device_name);
}
