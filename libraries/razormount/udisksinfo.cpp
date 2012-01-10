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
 
#include <QDBusInterface>
#include <QStringList>

#include "udisksinfo.h"


UdisksInfo::UdisksInfo(QDBusObjectPath device, QObject *parent)
    : QObject(parent),
      m_size(-1),
      m_type(UdisksInfo::Unknown),
      m_isExternal(false),
      m_isMounted(false),
      m_isEjectable(false)
{
    m_dbus = new QDBusInterface ("org.freedesktop.UDisks",
                             device.path(),
                             "org.freedesktop.UDisks.Device",
                             QDBusConnection::systemBus(),
                             this);

    // display name
    foreach (const char *i, QList<const char*>() << "IdLabel" << "DeviceFile" << "IdUuid")
    {
        m_displayName = m_dbus->property(i).toString();
        if (!m_displayName.isEmpty())
            break;
    }

    if (!m_dbus->property("DeviceIsSystemInternal").toBool()
        && m_dbus->property("DeviceIsMediaAvailable").toBool()
        && m_dbus->property("IdUsage").toString() == "filesystem")
    {
        m_isExternal = true;
    }

    m_isMounted = m_dbus->property("DeviceIsMounted").toBool();

    if (m_dbus->property("DeviceIsDrive").toBool())
    {
        m_isEjectable = m_dbus->property("DriveIsMediaEjectable").toBool();
    }

    m_size = m_dbus->property("DeviceSize").toULongLong();
    m_fileSystem = m_dbus->property("IdType").toString();

    if (m_isMounted)
    {
        QStringList paths = m_dbus->property("DeviceMountPaths").toStringList();
        if (!paths.empty())
            m_path = paths.at(0);
    }

    // TODO/FIXME: more types
    m_type = m_dbus->property("DeviceIsOpticalDisc").toBool() ? UdisksInfo::Optical : UdisksInfo::USB;
}

bool UdisksInfo::mount()
{
    if (m_isMounted)
        return true;

    QList<QVariant> args;
    args << QVariant(QString()) << QVariant(QStringList());

    bool ret;
    ret = m_dbus->callWithCallback("FilesystemMount", args, this,
                             SLOT(m_dbus_success(QDBusMessage)),
                             SLOT(m_dbus_error(QDBusError, QDBusMessage)));
    QStringList paths = m_dbus->property("DeviceMountPaths").toStringList();
    if (!paths.empty())
        m_path = paths.at(0);
    
    return ret;
}


bool UdisksInfo::unmount()
{
    if (!m_isMounted)
        return true;

    QList<QVariant> args;
    args << QVariant(QStringList());

    bool ret;
    ret = m_dbus->callWithCallback("FilesystemUnmount", args, this,
                             SLOT(m_dbus_success(QDBusMessage)),
                             SLOT(m_dbus_error(QDBusError, QDBusMessage)));
    // TODO/FIXME: should it be here?
    if (m_isEjectable)
        ret |= eject();
    
    return ret;
}


bool UdisksInfo::eject()
{
    if (!m_isMounted)
        return true;

    QList<QVariant> args;
    args << QVariant(QStringList());

    return m_dbus->callWithCallback("DriveEject", args, this,
                             SLOT(m_dbus_success(QDBusMessage)),
                             SLOT(m_dbus_error(QDBusError, QDBusMessage)));
}


void UdisksInfo::m_dbus_success(const QDBusMessage &msg)
{
    emit success();
}


void UdisksInfo::m_dbus_error(const QDBusError &err, const QDBusMessage &msg)
{
    qDebug() << "UdisksInfo::m_dbus_error" << err.message();
    emit error(err.message());
}

QString UdisksInfo::iconName() const
{
    // TODO/FIXME: more device types!
    switch (m_type)
    {
        case UdisksInfo::Optical:
            return "media-optical";
        case UdisksInfo::USB:
            return "drive-removable-media-usb-pendrive";
        case UdisksInfo::Unknown:
        default:
            return QString();
    }
#if 0
    /* raw_info example:
    QMap(("DEVLINKS", "/dev/scd0 /dev/disk/by-id/ata-VBOX_CD-ROM_VB2-01700376 /dev/disk/by-path/pci-0000:00:01.1-scsi-1:0:0:0 /dev/disk/by-label/FreeBSD_Install /dev/cdrom /dev/dvd")("DEVNAME", "/dev/sr0")("DEVPATH", "/devices/pci0000:00/0000:00:01.1/host2/target2:0:0/2:0:0:0/block/sr0")("DEVTYPE", "disk")("GENERATED", "1")("ID_ATA", "1")("ID_BUS", "ata")("ID_CDROM", "1")("ID_CDROM_DVD", "1")("ID_CDROM_MEDIA", "1")("ID_CDROM_MEDIA_CD", "1")("ID_CDROM_MEDIA_SESSION_COUNT", "1")("ID_CDROM_MEDIA_TRACK_COUNT", "1")("ID_CDROM_MEDIA_TRACK_COUNT_DATA", "1")("ID_CDROM_MRW", "1")("ID_CDROM_MRW_W", "1")("ID_FS_LABEL", "FreeBSD_Install")("ID_FS_LABEL_ENC", "FreeBSD_Install")("ID_FS_TYPE", "iso9660")("ID_FS_USAGE", "filesystem")("ID_MODEL", "VBOX_CD-ROM")("ID_MODEL_ENC", "VBOX\x20CD-ROM\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20")("ID_PATH", "pci-0000:00:01.1-scsi-1:0:0:0")("ID_REVISION", "1.0")("ID_SERIAL", "VBOX_CD-ROM_VB2-01700376")("ID_SERIAL_SHORT", "VB2-01700376")("ID_TYPE", "cd")("MAJOR", "11")("MINOR", "0")("REMOVABLE", "1")("SUBSYSTEM", "block")("TAGS", ":udev-acl:")("UDEV_LOG", "3")("UDISKS_PARTITION_SIZE", "")("UDISKS_PRESENTATION_NOPOLICY", "0")) 
    */
    QString DEVTYPE = m_dbus->property("DEVTYPE").toString();
    QString ID_TYPE = m_dbus->property("ID_TYPE").toString();
    QString ID_CDROM_MEDIA_STATE = m_dbus->property("ID_CDROM_MEDIA_STATE").toString();
    QString ID_CDROM_MEDIA_CD_R = m_dbus->property("ID_CDROM_MEDIA_CD_R").toString();
    QString DKD_MEDIA_AVAILABLE = m_dbus->property("DKD_MEDIA_AVAILABLE").toString();
    QString ID_CDROM_MEDIA = m_dbus->property("ID_CDROM_MEDIA").toString();
    QString ID_CDROM_DVD = m_dbus->property("ID_CDROM_DVD").toString();
    QString DKD_PRESENTATION_ICON_NAME = m_dbus->property("DKD_PRESENTATION_ICON_NAME").toString();
    // based on: http://wiki.xfce.org/dev/thunar-volman-udev
    // Removable Drives (CD/DVD/Blu-ray drives, no mass storage devices)
    if (DEVTYPE == "disk" && ID_TYPE == "cd")
        return "media-optical";

    else if (DEVTYPE == "disk" && ID_TYPE == "dvd")
        return "media-optical-dvd";

    else if (DEVTYPE == "disk" && ID_TYPE == "blur-ay") // just guessing here
        return "media-optical-blu-ray";

    else if (DEVTYPE == "disk" && ID_TYPE == "dvd")
        return "media-optical-dvd";

    // Removable HDD
    else if (DEVTYPE == "partition" && ID_TYPE == "disk")
        return "drive-removable-media-usb";

    // Removable Drive
    else if (DEVTYPE == "disk" && ID_TYPE == "disk")
        return "drive-removable-media-usb-pendrive";

    // Blank CDs and DVDs
    else if (ID_CDROM_MEDIA_STATE == "blank" && ID_CDROM_MEDIA_CD_R == "1")
        return "media-optical-recordable";

    // Audio CDs
    else if (DKD_MEDIA_AVAILABLE == "1" && ID_CDROM_MEDIA == "1")
        return "media-optical-audio";

    // DVDs
    else if (DKD_MEDIA_AVAILABLE == "1" && ID_CDROM_DVD == "1")
        return "media-optical-dvd-video";

    // Portable Music Players
    else if (DKD_PRESENTATION_ICON_NAME == "multimedia-player")
        return "multimedia-player";

    return QString();
#endif
}
