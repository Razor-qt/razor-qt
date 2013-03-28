/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *   Petr Vanek <petr@scribus.info>
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

#include "rzmountproviders.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMessage>
#include <QtCore/QDebug>


/************************************************

 ************************************************/
RzMountProvider::RzMountProvider(QObject *parent):
    QObject(parent),
    mIsValid(false)
{
}


/************************************************

 ************************************************/
UDiskProvider::UDiskProvider(QObject *parent):
    RzMountProvider(parent)
{
    QDBusConnection system = QDBusConnection::systemBus();

    if (!system.isConnected())
    {
        return;
    }

    system.connect("org.freedesktop.UDisks2",
                    "/org/freedesktop/UDisks2",
                    "org.freedesktop.DBus.ObjectManager",
                    "InterfacesAdded",
                    this,
                    SLOT(dbusDeviceAdded(QDBusObjectPath, QStringList)));

    system.connect("org.freedesktop.UDisks2",
                    "/org/freedesktop/UDisks2",
                    "org.freedesktop.DBus.ObjectManager",
                    "InterfacesRemoved",
                    this,
                    SLOT(dbusDeviceRemoved(QDBusObjectPath, QStringList)));
/*
    system.connect("org.freedesktop.UDisks",
                    "/org/freedesktop/UDisks",
                    "org.freedesktop.UDisks",
                    "DeviceChanged",
                    this,
                    SLOT(dbusDeviceChanged(QDBusObjectPath)));
                    */

    mIsValid = true;
}


/************************************************

 ************************************************/
void UDiskProvider::update()
{

    QDBusInterface devEnum("org.freedesktop.UDisks",
                           "/org/freedesktop/UDisks",
                           "org.freedesktop.UDisks",
                           QDBusConnection::systemBus());

    QDBusMessage enumRes = devEnum.call("EnumerateDevices");
    if (enumRes.type() != QDBusMessage::ReplyMessage || !enumRes.arguments().at(0).canConvert<QDBusArgument>())
    {
        if (enumRes.type() == QDBusMessage::ErrorMessage)
            qWarning() << "ERROR: Can't call EnumerateDevices"
                       <<  qPrintable(enumRes.errorName()) << qPrintable(enumRes.errorMessage());
        else
            qWarning() << "ERROR: Unexpected result type of EnumerateDevices call";
        return;
    }

    const QDBusArgument enumArg = enumRes.arguments().at(0).value<QDBusArgument>();
    if (enumArg.currentType() != QDBusArgument::ArrayType)
    {
        qWarning() << "ERROR: Unexpected argument type of EnumerateDevices call";
        return;
    }

    enumArg.beginArray();
    while (!enumArg.atEnd())
    {
        QDBusObjectPath path = qdbus_cast<QDBusObjectPath>(enumArg);

        if (mDevicesByPath.contains(path.path()))
            dbusDeviceChanged(path);
        else
            dbusDeviceAdded(path);
    }
    enumArg.endArray();

}


/************************************************

 ************************************************/
UDiskMountDevice *UDiskProvider::getDevice(const QDBusObjectPath &path) const
{
    if (mDevicesByPath.contains(path.path()))
        return mDevicesByPath[path.path()];
    else
        return 0;
}


/************************************************

 ************************************************/
void UDiskProvider::addDevice(UDiskMountDevice *device)
{
    mDevicesByPath.insert(device->udiskPath(), device);
    mDevices.append(device);
}


/************************************************

 ************************************************/
void UDiskProvider::delDevice(UDiskMountDevice *device)
{
    mDevices.removeAll(device);
    mDevicesByPath.remove(device->udiskPath());
    device->deleteLater();
}


/************************************************

 ************************************************/
void UDiskProvider::dbusDeviceAdded(const QDBusObjectPath &path)
{
    UDiskMountDevice *device = new UDiskMountDevice(path);

     addDevice(device);
     emit deviceAdded(device);
}


/************************************************

 ************************************************/
void UDiskProvider::dbusDeviceRemoved(const QDBusObjectPath &path)
{
    UDiskMountDevice *device = getDevice(path);
    if (device)
    {
        emit deviceRemoved(device);
        delDevice(device);
    }
}


/************************************************

 ************************************************/
void UDiskProvider::dbusDeviceChanged(const QDBusObjectPath &path)
{
    UDiskMountDevice *device = getDevice(path);
    if (!device)
        return;

    if (device->update())
        emit deviceChanged(device);
}


/************************************************

 ************************************************/
UDiskMountDevice::UDiskMountDevice(const QDBusObjectPath &path):
    RazorMountDevice(),
    mUdiskPath(path.path())
{
    mDbus = new QDBusInterface("org.freedesktop.UDisks",
                               path.path(),
                               "org.freedesktop.UDisks.Device",
                               QDBusConnection::systemBus(),
                               this);
    update();
}


/************************************************

 ************************************************/
bool UDiskMountDevice::update()
{
    bool res = false;
    res = setDevFile(mDbus->property("DeviceFile").toString()) || res;

    res = setMediaType(calcMediaType()) || res;
    res = setLabel(calcLabel()) || res;
    res = setIsExternal(calcIsExternal()) || res;
    res = setIconName(calcIconName()) || res;

    res = setIsMounted(mDbus->property("DeviceIsMounted").toBool()) || res;

    if (mDbus->property("DeviceIsDrive").toBool())
        res = setIsEjectable(mDbus->property("DriveIsMediaEjectable").toBool());

    res = setSize(mDbus->property("DeviceSize").toULongLong()) || res;
    res = setVendor(mDbus->property("DriveVendor").toString()) || res;
    res = setModel(mDbus->property("DriveModel").toString()) || res;
    res = setFileSystem(mDbus->property("IdType").toString()) || res;

    QStringList paths = mDbus->property("DeviceMountPaths").toStringList();
    if (!paths.empty())
        res = setMountPath(paths.first()) || res;

    if (res)
        emit changed();

    return res;
}


/************************************************

 ************************************************/
RazorMountDevice::MediaType UDiskMountDevice::calcMediaType()
{
    if (mDbus->property("DeviceIsOpticalDisc").toBool())
        return RazorMountDevice::MediaTypeOptical;

    const QString media = mDbus->property("DriveMedia").toString();
    const QString mediaCompat = mDbus->property("DriveMediaCompatibility").toString();
    const QString idUsage = mDbus->property("IdUsage").toString();

    if (mDbus->property("DeviceIsDrive").toBool())
    {
        if (mediaCompat == "floppy")
            return RazorMountDevice::MediaTypeFdd;

        if (idUsage == "filesystem")
            return RazorMountDevice::MediaTypeDrive;

        return RazorMountDevice::MediaTypeUnknown;
    }

    if (mDbus->property("DeviceIsPartition").toBool())
    {
        if (idUsage == "filesystem")
            return RazorMountDevice::MediaTypePartition;

        return RazorMountDevice::MediaTypeUnknown;
    }

    return RazorMountDevice::MediaTypeUnknown;
}


/************************************************

 ************************************************/
QString UDiskMountDevice::calcLabel()
{
    const QString idLabel = mDbus->property("IdLabel").toString();

    if (mMediaType ==  MediaTypeFdd)
        return tr("Floppy drive");

    if (mMediaType ==  MediaTypeOptical)
        return idLabel;


    const QString driveVendor = mDbus->property("DriveVendor").toString();
    const QString driveModel  = mDbus->property("DriveModel").toString();
    const qulonglong size = mDbus->property("DeviceSize").toULongLong();

    QString label;
    if (!idLabel.isEmpty())
    {
        label = idLabel;
    }
    else
    {
       if (!driveVendor.isEmpty())
            label = driveVendor;

       if (!driveModel.isEmpty())
            label += QString(" - %1").arg(driveModel);
    }

    if (label.isEmpty())
        label = mDevFile;

    if (mSize)
        label += QString(" [%3]").arg(sizeToString(size));

    return label;
}


/************************************************

 ************************************************/
bool UDiskMountDevice::calcIsExternal()
{
    return ! mDbus->property("DeviceIsSystemInternal").toBool();
//    if (mDbus->property("DeviceIsSystemInternal").toBool())
//        return false;

//    if (mDbus->property("DeviceIsMediaAvailable").toBool()
//        && mDbus->property("IdUsage").toString() == "filesystem")
//        return true;

//    if (mDbus->property("DeviceIsDrive").toBool())
//    {
//        return true;
//    }

 //   return false;
}


/************************************************

 ************************************************/
QString UDiskMountDevice::calcIconName()
{
    const QString media = mDbus->property( "DriveMedia" ).toString();

    switch (mMediaType)
    {
    // ..............................................
    case MediaTypeDrive:
    case MediaTypePartition:
        {
            // handle drives
            const QString conn = mDbus->property( "DriveConnectionInterface" ).toString();

            if (conn == "usb")
                return "drive-removable-media-usb";

            return "drive-removable-media";
        }

    // ..............................................
    case MediaTypeFdd:
        {
            return "media-floppy";
        }


    // ..............................................
//    case MediaTypeFlash:
//        {
//            if ( media == "flash_ms" ) // Flash & Co.
//                return "media-flash-memory-stick";

//            if ( media == "flash_sd" ||
//                 media == "flash_sdhc" ||
//                 media == "flash_mmc" )
//                return "media-flash-sd-mmc";

//            if ( media == "flash_sm" )
//                return "media-flash-smart-media";

//            return "media-flash";
//        }


    // ..............................................
    case MediaTypeOptical:
        {
            bool isWritable = mDbus->property( "OpticalDiscIsBlank" ).toBool() ||
                              mDbus->property("OpticalDiscIsAppendable").toBool();

            if (isWritable)
                return "media-optical-recordable";

            if (media.startsWith("optical_dvd") ||
                media.startsWith( "optical_hddvd")) // DVD
                return "media-optical-dvd";

            if (media.startsWith("optical_bd")) // BluRay
                return "media-optical-blu-ray";

            return "media-optical";
        }

    // ..............................................
    case MediaTypeUnknown:
        {
            return "drive-harddisk";
        }
    }

     return "drive-harddisk";
}


/************************************************

 ************************************************/
void UDiskMountDevice::dbusError(const QDBusError &err, const QDBusMessage &msg)
{
    qWarning() << "UdisksInfo::mDbus_error" << err.message();
    emit error(err.message());
}


/************************************************

 ************************************************/
bool UDiskMountDevice::mount()
{
    if (mIsMounted)
        return true;

    QList<QVariant> args;
    args << QVariant(QString()) << QVariant(QStringList());

    bool ret;
    ret = mDbus->callWithCallback("FilesystemMount", args, this,
                             //SLOT(dbusSuccess(QDBusMessage)),
                             SIGNAL(mounted()),
                             SLOT(dbusError(QDBusError, QDBusMessage)));

    QStringList paths = mDbus->property("DeviceMountPaths").toStringList();

    if (!paths.empty())
        mMountPath = paths.at(0);
    else
        mMountPath = "";

    return ret;
}


/************************************************

 ************************************************/
bool UDiskMountDevice::unmount()
{
    if (!mIsMounted)
        return true;

    QList<QVariant> args;
    args << QVariant(QStringList());

    bool ret;
    ret = mDbus->callWithCallback("FilesystemUnmount", args, this,
//                             SLOT(dbusSuccess(QDBusMessage)),
                             SIGNAL(unmounted()),
                             SLOT(dbusError(QDBusError, QDBusMessage)));
    return ret;
}


/************************************************

 ************************************************/
bool UDiskMountDevice::eject()
{
    if (!mIsMounted)
        return false;

    QList<QVariant> args;
    args << QVariant(QStringList());

    return mDbus->callWithCallback("DriveEject", args, this,
                             SLOT(dbusSuccess(QDBusMessage)),
                             SLOT(dbusError(QDBusError, QDBusMessage)));
}
