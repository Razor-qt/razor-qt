/* BEGIN_COMMON_COPYRIGHT_HEADER
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

#ifndef STORAGEITEM_H
#define STORAGEITEM_H

#include <QObject>

#include "diskmonitor.h"

typedef enum {
    RC_MOUNT,
    RC_UNMOUNT
} RunCommand;

class StorageItem : public QObject
{
    Q_OBJECT
public:
    explicit StorageItem(DiskInfo info, QObject *parent = 0);
    ~StorageItem();

    bool    isMounted();
    QString getMountPoint();

    void   setMountStatus(bool is_mounted, const QString &mount_point = QString());

    bool    mount(QString &text_status);
    bool    unmount(QString &text_status);

    DiskInfo diskInfo() const  { return _udev_info; }
signals:
    void deviceMounted(QString device_name);
    void deviceUnmounted(QString device_name);
    void deviceError(QString device_name, QString error_text);

private:
    DiskInfo  _udev_info;
    QString   _mount_point;
    QString   _mount_options;
    bool      _is_mounted;
};

#endif // STORAGEITEM_H
