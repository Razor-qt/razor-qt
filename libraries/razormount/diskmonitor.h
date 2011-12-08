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

#ifndef DISKMONITOR_H
#define DISKMONITOR_H

#include <libudev.h>

#include <QThread>
#include <QMap>

typedef QMap<QString, QString> QUdevInfo;

typedef struct _DiskInfo {
    _DiskInfo()
    {
    }

    QString name;               // Unified field for display device name
    QString device_name;        // Full device name with path like '/dev/sda1'
    QString model_name;         // Hardware model like 'ST9160827AS'
    QString file_system_type;   // File system name like 'vfat'
    QString file_system_label;  // Label, if set
    QString file_system_size;   // Only if found udisks entries

    /* raw_info example:
    QMap(("DEVLINKS", "/dev/scd0 /dev/disk/by-id/ata-VBOX_CD-ROM_VB2-01700376 /dev/disk/by-path/pci-0000:00:01.1-scsi-1:0:0:0 /dev/disk/by-label/FreeBSD_Install /dev/cdrom /dev/dvd")("DEVNAME", "/dev/sr0")("DEVPATH", "/devices/pci0000:00/0000:00:01.1/host2/target2:0:0/2:0:0:0/block/sr0")("DEVTYPE", "disk")("GENERATED", "1")("ID_ATA", "1")("ID_BUS", "ata")("ID_CDROM", "1")("ID_CDROM_DVD", "1")("ID_CDROM_MEDIA", "1")("ID_CDROM_MEDIA_CD", "1")("ID_CDROM_MEDIA_SESSION_COUNT", "1")("ID_CDROM_MEDIA_TRACK_COUNT", "1")("ID_CDROM_MEDIA_TRACK_COUNT_DATA", "1")("ID_CDROM_MRW", "1")("ID_CDROM_MRW_W", "1")("ID_FS_LABEL", "FreeBSD_Install")("ID_FS_LABEL_ENC", "FreeBSD_Install")("ID_FS_TYPE", "iso9660")("ID_FS_USAGE", "filesystem")("ID_MODEL", "VBOX_CD-ROM")("ID_MODEL_ENC", "VBOX\x20CD-ROM\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20")("ID_PATH", "pci-0000:00:01.1-scsi-1:0:0:0")("ID_REVISION", "1.0")("ID_SERIAL", "VBOX_CD-ROM_VB2-01700376")("ID_SERIAL_SHORT", "VB2-01700376")("ID_TYPE", "cd")("MAJOR", "11")("MINOR", "0")("REMOVABLE", "1")("SUBSYSTEM", "block")("TAGS", ":udev-acl:")("UDEV_LOG", "3")("UDISKS_PARTITION_SIZE", "")("UDISKS_PRESENTATION_NOPOLICY", "0")) 
    */
    QUdevInfo raw_info;         // Associtive massive of all udev's ops
    QString iconName() const;
} DiskInfo;

class DiskMonitor : public QThread
{
    Q_OBJECT
public:
    explicit DiskMonitor(QObject *parent = 0);
    ~DiskMonitor();

    QList<DiskInfo*> scanDevices();

signals:
    void deviceConnected(DiskInfo info);
    void deviceDisconnected(DiskInfo info);

public slots:

private:
    DiskInfo             _disk_info;
    struct udev         *_udev;
    struct udev_monitor *_monitor;

private:
    void run();
    void init();
    void fillDiskInfo(struct udev_device *device);
    void fillDiskInfo(struct udev_device *device, DiskInfo &info);

};


QDebug operator<<(QDebug dbg, const _DiskInfo &info);
QDebug operator<<(QDebug dbg, const _DiskInfo *info);
#endif // DISKMONITOR_H
