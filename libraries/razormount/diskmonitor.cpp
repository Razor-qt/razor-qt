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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>

#include <iostream>

#include <QStringList>
#include <QFileInfo>
#include <QMetaType>
#include <QDebug>

#include "diskmonitor.h"

QString DiskInfo::iconName() const
{
    // based on: http://wiki.xfce.org/dev/thunar-volman-udev
    // Removable Drives (CD/DVD/Blu-ray drives, no mass storage devices)
    if (raw_info["DEVTYPE"] == "disk" && raw_info["ID_TYPE"] == "cd")
        return "media-optical";

    else if (raw_info["DEVTYPE"] == "disk" && raw_info["ID_TYPE"] == "dvd")
        return "media-optical-dvd";

    else if (raw_info["DEVTYPE"] == "disk" && raw_info["ID_TYPE"] == "blur-ay") // just guessing here
        return "media-optical-blu-ray";

    else if (raw_info["DEVTYPE"] == "disk" && raw_info["ID_TYPE"] == "dvd")
        return "media-optical-dvd";

    // Removable HDD
    else if (raw_info["DEVTYPE"] == "partition" && raw_info["ID_TYPE"] == "disk")
        return "drive-removable-media-usb";

    // Removable Drive
    else if (raw_info["DEVTYPE"] == "disk" && raw_info["ID_TYPE"] == "disk")
        return "drive-removable-media-usb-pendrive";

    // Blank CDs and DVDs
    else if (raw_info["ID_CDROM_MEDIA_STATE"] == "blank" && raw_info["ID_CDROM_MEDIA_CD_R"] == "1")
        return "media-optical-recordable";

    // Audio CDs
    else if (raw_info["DKD_MEDIA_AVAILABLE"] == "1" && raw_info["ID_CDROM_MEDIA"] == "1")
        return "media-optical-audio";

    // DVDs
    else if (raw_info["DKD_MEDIA_AVAILABLE"] == "1" && raw_info["ID_CDROM_DVD"] == "1")
        return "media-optical-dvd-video";

    // Portable Music Players
    else if (raw_info["DKD_PRESENTATION_ICON_NAME"] == "multimedia-player")
        return "multimedia-player";

    return QString();
}

DiskMonitor::DiskMonitor(QObject *parent) : QThread(parent)
{
    _udev    = NULL;
    _monitor = NULL;

    qRegisterMetaType<DiskInfo>("DiskInfo");

    init();
}

DiskMonitor::~DiskMonitor()
{
    udev_monitor_unref(_monitor);
    udev_unref(_udev);
}

void DiskMonitor::init()
{
    int status = -1;

    if (_udev == NULL)
        _udev    = udev_new();

    if (_udev == NULL)
    {
        std::cout << "Can't init udev" << std::endl;
        return;
    }

    if (_monitor == NULL)
    {
        _monitor = udev_monitor_new_from_netlink(_udev, "udev");
        if (_monitor == NULL)
        {
            std::cout << "Can't init monitor" << std::endl;
            return;
        }

        status = udev_monitor_filter_add_match_subsystem_devtype(_monitor, "block", NULL);
        if (status < 0)
        {
            std::cout << "Can't add filter for block" << std::endl;
        }
    }
}

void DiskMonitor::run()
{
    std::cout << "Disk Monitor started" << std::endl;

    int    enable_status = -1;
    fd_set read_fds;

    while(1)
    {
        int                 fd_count;
        int                 monitor_fd;
        struct udev_device *device;

        if (_udev == NULL || _monitor == NULL)
        {
            std::cout << "Monitor is not initialized... try init...\n";
            init();
            continue;
        }

        if (enable_status == -1)
        {
            enable_status = udev_monitor_enable_receiving(_monitor);
            if (enable_status == -1)
            {
                std::cout << "Can't enable receiving... try later...\n";
            }
            continue;
        }

        monitor_fd = udev_monitor_get_fd(_monitor);
        FD_ZERO(&read_fds);
        FD_SET(monitor_fd, &read_fds);

        fd_count = select(monitor_fd + 1, &read_fds, NULL, NULL, NULL);
        if (fd_count < 0)
        {
            continue;
        }

        if (!FD_ISSET(monitor_fd, &read_fds))
        {
            continue;
        }

        device = udev_monitor_receive_device(_monitor);
        if (device == NULL)
        {
            continue;
        }

        QString action     = udev_device_get_action(device);
        QString new_action = action;
        QString devnode    = udev_device_get_devnode(device);

        fillDiskInfo(device);

        if (action == "change")
        {
            // For CD/DVD
            if (_disk_info.raw_info["ID_TYPE"]              == "cd" &&
                _disk_info.raw_info["ID_CDROM_MEDIA_STATE"] != "blank") // ignore blanks disks
            {
                if (_disk_info.raw_info["ID_CDROM_MEDIA"] == "1") // Media present
                {
                    new_action = "add";
                }
                else
                {
                    new_action = "remove";
                }
            }
        }

        if (_disk_info.raw_info["ID_FS_USAGE"] == "filesystem" ||
            _disk_info.raw_info["ID_TYPE"]     == "cd")
        {
            if (new_action == "add")
            {
                // emit signal
                emit deviceConnected(_disk_info);
            }
            else if (new_action == "remove")
            {
                // emit signal
                emit deviceDisconnected(_disk_info);
            }
        }

        udev_device_unref(device);
    }
}

void DiskMonitor::fillDiskInfo(struct udev_device *device)
{
    fillDiskInfo(device, _disk_info);
}

void DiskMonitor::fillDiskInfo(struct udev_device *device, DiskInfo &info)
{
    struct udev_list_entry *property_list = NULL;
    struct udev_list_entry *entry         = NULL;

    info.raw_info.clear();

    property_list = udev_device_get_properties_list_entry(device);
    udev_list_entry_foreach(entry, property_list)
    {
        QString property_name  = udev_list_entry_get_name(entry);
        QString property_value = QString::fromLocal8Bit(udev_device_get_property_value(device,
                                                                property_name.toAscii().data()));

        info.raw_info[property_name] = property_value;
    }

    info.device_name       = info.raw_info["DEVNAME"];
    info.model_name        = info.raw_info["ID_MODEL"];
    info.file_system_type  = info.raw_info["ID_FS_TYPE"];
    info.file_system_label = info.raw_info["ID_FS_LABEL"];
    info.file_system_size  = info.raw_info["UDISKS_PARTITION_SIZE"];

    if (!info.file_system_label.isEmpty())
    {
        info.name = info.file_system_label;
    }
    else
    {
        info.name = QString("%1 %2")
                        .arg(info.raw_info["ID_VENDOR"])
                        .arg(info.raw_info["ID_MODEL"]);
    }

    info.raw_info.insert("REMOVABLE", "0");

    // Check removable attr
    struct udev_device *parent = 0;
    if (info.raw_info["DEVTYPE"] == "partition")
    {
        parent = udev_device_get_parent(device);
    }
    else if (info.raw_info["DEVTYPE"] == "disk")
    {
        parent = device; // disk without partitions
    }

    if (parent != 0)
    {
        const char *value = udev_device_get_sysattr_value(parent, "removable");

        if (value != NULL)
        {
            info.raw_info["REMOVABLE"] = value;
        }
    }

}

#warning "TODO: disk without partition (like Digma e600 Book Reader) detection is wrong." \
         "udev provide very short info, so I can detect FS on this partitions." \
         "Runtime detection (connect/disconnect) work well."
QList<DiskInfo *> DiskMonitor::scanDevices()
{
    DiskInfo          *disk;
    QList<DiskInfo *>  disks;

    struct udev_enumerate  *enumerate  = udev_enumerate_new(_udev);

    if (enumerate == NULL)
    {
        std::cout << "Can't init enumerate" << std::endl;
        return disks;
    }

    udev_enumerate_add_match_subsystem(enumerate, "block");
    udev_enumerate_scan_devices(enumerate);

    struct udev_device     *device     = NULL;
    struct udev_list_entry *entryes    = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *entry;

    udev_list_entry_foreach(entry, entryes)
    {
        device = udev_device_new_from_syspath(_udev, udev_list_entry_get_name(entry));
        if (device == NULL)
        {
            continue;
        }

        disk = new DiskInfo();
        fillDiskInfo(device, *disk);

        //const char *name  = udev_list_entry_get_name(entry);

        //std::cout << "Entry:       " << name << std::endl;
        //std::cout << "isRemovable: "
        //    << (!disk->raw_info["REMOVABLE"].isEmpty() ? qPrintable(disk->raw_info["REMOVABLE"]) : "unknown")
        //    << std::endl;

        if (disk->raw_info["ID_FS_USAGE"] == "filesystem" &&
            disk->raw_info["REMOVABLE"]   == "1")
        {
            disks.append(disk);
        }
        else if (disk->raw_info["ID_TYPE"]              == "cd" &&
                 disk->raw_info["ID_CDROM_MEDIA"]       == "1"  &&
                 disk->raw_info["ID_CDROM_MEDIA_STATE"] != "blank")
        {
            disks.append(disk);
        }
        else
        {
            delete disk;
        }

        udev_device_unref(device);
    }

    udev_enumerate_unref(enumerate);

    return disks;
}

QDebug operator<<(QDebug dbg, const _DiskInfo &info)
{

    QString name;               // Unified field for display device name
    QString device_name;        // Full device name with path like '/dev/sda1'
    QString model_name;         // Hardware model like 'ST9160827AS'
    QString file_system_type;   // File system name like 'vfat'
    QString file_system_label;  // Label, if set
    QString file_system_size;

    QString s;
    s+=QString("name: %1\n").arg(info.name);
    s+=QString("device_name: %1\n").arg(info.device_name);
    s+=QString("model_name: %1\n").arg(info.model_name);
    s+=QString("file_system_type: %1\n").arg(info.file_system_type);
    s+=QString("file_system_label: %1\n").arg(info.file_system_label);
    s+=QString("file_system_size: %1\n").arg(info.file_system_size);

    s+=QString("raw_info:\n");

    QMapIterator<QString, QString> i(info.raw_info);
    while (i.hasNext()) {
         i.next();
         s+=QString("  %1: %2\n").arg(i.key(), i.value());
     }

    dbg.nospace() << s;
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const _DiskInfo *info)
{
    return operator<<(dbg, *info);
}
