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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
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

#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include <QObject>
#include <QMap>

#include "storageitem.h"
#include "diskmonitor.h"

class StorageManager : public QObject
{
    Q_OBJECT
public:
    explicit StorageManager(QObject *parent = 0);
    virtual ~StorageManager();

    void addDevice(const    DiskInfo &info);
    void removeDevice(const DiskInfo &info);
    void removeDevice(const QString  &dev_name);

    QList<StorageItem *> getDevices();
    StorageItem         *getDevice(const QString  &dev_name);
    StorageItem         *getDevice(const DiskInfo &info);

signals:

public slots:

private:
    QMap<QString, StorageItem *> _storage_items;
};

#endif // STORAGEMANAGER_H
