/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2013 Razor team
 * Authors:
 *   Petr Vanek <petr@yarpen.cz>
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

#include <QtDBus>
#include "rzmountproviders.h"


UDisks2Provider::UDisks2Provider(QObject *parent):
    RzMountProvider(parent)
{
    QDBusConnection system = QDBusConnection::systemBus();

    if (!system.isConnected())
    {
        qDebug() << "UDisks2Provider::UDisks2Provider - not connected - " << mIsValid;
        return;
    }

    bool ret;
    ret = system.connect("org.freedesktop.UDisks2",
                         "/org/freedesktop/UDisks2",
                         "org.freedesktop.UDisks2",
                         "InterfacesAdded",
                         this,
                         SLOT(dbusDeviceAdded(QDBusObjectPath,QVariantMap)));
    if (!ret)
    {
        qDebug() << "UDisks2Provider::UDisks2Provider InterfacesAdded connect failed";
        return;
    }

    ret = system.connect("org.freedesktop.UDisks2",
                         "/org/freedesktop/UDisks2",
                         "org.freedesktop.UDisks2",
                         "InterfacesRemoved",
                         this,
                        SLOT(dbusDeviceRemoved(QDBusObjectPath,QStringList)));
    if (!ret)
    {
        qDebug() << "UDisks2Provider::UDisks2Provider InterfacesRemoved connect failed";
        return;
    }

    mIsValid = true;
    qDebug() << "UDisks2Provider::UDisks2Provider final validity" << mIsValid;

}

void UDisks2Provider::update()
{
    qDebug() << "UDisks2Provider::update() called";

    QList<QDBusObjectPath> paths;
    QDBusMessage call = QDBusMessage::createMethodCall("org.freedesktop.UDisks2",
                                                       "/org/freedesktop/UDisks2/block_devices",
                                                       "org.freedesktop.DBus.Introspectable",
                                                       "Introspect");
    QDBusPendingReply<QString> reply = QDBusConnection::systemBus().call(call);

    if (!reply.isValid())
    {
        qWarning("UDisks2Manager: error: %s", qPrintable(reply.error().name()));
        return;
    }

    QXmlStreamReader xml(reply.value());
    while (!xml.atEnd())
    {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name().toString() == "node" )
        {
            QString name = xml.attributes().value("name").toString();
            if(!name.isEmpty())
                paths << QDBusObjectPath("/org/freedesktop/UDisks2/block_devices/" + name);
        }
    }

    foreach (QDBusObjectPath i, paths)
    {
//        qDebug() << "UDisks2Provider::update() path:" << i.path();
        if (mDevicesByPath.contains(i.path()))
            dbusDeviceChanged(i);
        else
            dbusDeviceAdded(i, QVariantMap());
    }
    
//    qDebug() << mDevicesByPath;
}
 
UDisks2MountDevice *UDisks2Provider::getDevice(const QDBusObjectPath &path) const
{
    if (mDevicesByPath.contains(path.path()))
        return mDevicesByPath[path.path()];
    else
        return 0;
}

void UDisks2Provider::addDevice(UDisks2MountDevice *device)
{
    mDevicesByPath.insert(device->path().path(), device);
    mDevices.append(device);
}

void UDisks2Provider::delDevice(UDisks2MountDevice *device)
{
    mDevices.removeAll(device);
    mDevicesByPath.remove(device->path().path());
    device->deleteLater();
}

void UDisks2Provider::dbusDeviceAdded(const QDBusObjectPath &path, const QVariantMap &)
{
    if(path.path().startsWith("/org/freedesktop/UDisks2/jobs"))
        return;

    UDisks2MountDevice *device = new UDisks2MountDevice(path);

    addDevice(device);
    emit deviceAdded(device);
}

void UDisks2Provider::dbusDeviceRemoved(const QDBusObjectPath &path, const QStringList &)
{
    if(path.path().startsWith("/org/freedesktop/UDisks2/jobs"))
        return;

    UDisks2MountDevice *device = getDevice(path);
    if (device)
    {
        emit deviceRemoved(device);
        delDevice(device);
    }
}

void UDisks2Provider::dbusDeviceChanged(const QDBusObjectPath &path)
{
#if 0
    if(path.path().startsWith("/org/freedesktop/UDisks2/jobs"))
        return;

    UDisks2MountDevice *device = getDevice(path);
    if (!device)
        return;

    if (device->update())
        emit deviceChanged(device);
#endif
}

UDisks2MountDevice::UDisks2MountDevice(const QDBusObjectPath &path):
    RazorMountDevice(),
    mPath(path)
{
//    qDebug() << "UDisks2MountDevice::UDisks2MountDevice path" << mPath.path();
    mBlockIface = new QDBusInterface("org.freedesktop.UDisks2",
                                     mPath.path(),
                                     "org.freedesktop.UDisks2.Block",
                                     QDBusConnection::systemBus(),
                                     this);
    Q_ASSERT(mBlockIface);

    QDBusObjectPath drive_object = mBlockIface->property("Drive").value<QDBusObjectPath>();
//    qDebug() << "UDisks2MountDevice::UDisks2MountDevice drive_object" << drive_object.path();

    QDBusConnection::systemBus().connect("org.freedesktop.UDisks2",
                                         mPath.path(),
                                         "org.freedesktop.DBus.Properties","PropertiesChanged",
                                         this,
                                         SIGNAL(update()));

    mDriveIface = new QDBusInterface("org.freedesktop.UDisks2",
                                     drive_object.path(),
                                     "org.freedesktop.UDisks2.Drive",
                                     QDBusConnection::systemBus(),
                                     this);
    Q_ASSERT(mDriveIface);

    update();
}

void UDisks2MountDevice::update()
{

    bool res = false;
    res = setDevFile(mBlockIface->property("Device").toByteArray().data()) || res;

    res = setMediaType(calcMediaType()) || res;
    res = setLabel(calcLabel()) || res;
    res = setIsExternal(calcIsExternal()) || res;
    res = setIconName(calcIconName()) || res;

    QStringList mounts = mountPoints();
    res = setIsMounted(mounts.count() != 0) || res;

    res = setIsEjectable(mDriveIface->property("Ejectable").toBool()) || res;
    res = setSize(mBlockIface->property("Size").toULongLong()) || res;
    res = setVendor(mDriveIface->property("Vendor").toString()) || res;
    res = setModel(mDriveIface->property("Model").toString()) || res;
    res = setFileSystem(mBlockIface->property("IdType").toString()) || res;

    if (!mounts.empty())
        res = setMountPath(mounts.first()) || res;

    if (res)
        emit changed();
}

QStringList UDisks2MountDevice::mountPoints() const
{
    QStringList points;
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2",
                                                          mPath.path(),
                                                          "org.freedesktop.DBus.Properties",
                                                          "Get");

    QList<QVariant> args;
    args << "org.freedesktop.UDisks2.Filesystem" << "MountPoints";
    message.setArguments(args);

    QDBusMessage reply = QDBusConnection::systemBus().call(message);

    QList<QByteArray> l;
    foreach (QVariant arg, reply.arguments())
        arg.value<QDBusVariant>().variant().value<QDBusArgument>() >> l;
    
    foreach (QByteArray p, l)
        points.append(p);

    qDebug() << "UDisks2MountDevice::mountPoints()" << points;
    return points;
}

RazorMountDevice::MediaType UDisks2MountDevice::calcMediaType()
{
    if (mDriveIface->property("Optical").toBool())
        return RazorMountDevice::MediaTypeOptical;

    const QString media = mDriveIface->property("Media").toString();
    const QStringList mediaCompat = mDriveIface->property("MediaCompatibility").toStringList();
    const QString idUsage = mBlockIface->property("IdUsage").toString();

    // TODO/FIXME: how to get it from udisks2?!
//    if (mDbus->property("DeviceIsDrive").toBool())
//    {
        // TODO/FIXME: just guessing...
        if (mediaCompat.contains("floppy"))
            return RazorMountDevice::MediaTypeFdd;

        if (idUsage == "filesystem")
            return RazorMountDevice::MediaTypeDrive;

        return RazorMountDevice::MediaTypeUnknown;
//    }

//    if (mDbus->property("DeviceIsPartition").toBool())
//    {
//        if (idUsage == "filesystem")
//            return RazorMountDevice::MediaTypePartition;

//      return RazorMountDevice::MediaTypeUnknown;
//    }

    return RazorMountDevice::MediaTypeUnknown;

}

QString UDisks2MountDevice::calcLabel()
{
    const QString idLabel = mBlockIface->property("IdLabel").toString();

    if (mMediaType == MediaTypeFdd)
        return tr("Floppy drive");

    if (mMediaType == MediaTypeOptical)
        return idLabel;


    const QString driveVendor = mDriveIface->property("Vendor").toString();
    const QString driveModel  = mDriveIface->property("Model").toString();
    const qulonglong size = mBlockIface->property("Size").toULongLong();

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

    qDebug() << "LABEL" << mPath.path() << label;
    return label;
}

bool UDisks2MountDevice::calcIsExternal()
{
    // TODO/FIXME: I'm really unsure here...
    return mDriveIface->property("Removable").toBool();
}

QString UDisks2MountDevice::calcIconName()
{
    const QString media = mDriveIface->property( "Media" ).toString();

    switch (mMediaType)
    {
    // ..............................................
    case MediaTypeDrive:
    case MediaTypePartition:
        {
            // handle drives
            const QString conn = mDriveIface->property( "ConnectionBus" ).toString();

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


    case MediaTypeOptical:
        {
            bool isWritable = mDriveIface->property( "OpticalBlank" ).toBool()
                                // TODO/FIXME: how in udisks2?
                             //||
                             // mDbus->property("OpticalDiscIsAppendable").toBool();
                                ;

            if (isWritable)
                return "media-optical-recordable";

            if (media.startsWith("optical_dvd") ||
                media.startsWith( "optical_hddvd")) // DVD
                return "media-optical-dvd";

            if (media.startsWith("optical_bd")) // BluRay
                return "media-optical-blu-ray";

            return "media-optical";
        }

    case MediaTypeUnknown:
        {
            return "drive-harddisk";
        }
    }

    return "drive-harddisk";
}

void UDisks2MountDevice::dbusError(const QDBusError &err, const QDBusMessage &msg)
{
    qWarning() << "UDisks2MountDevice::mDbus_error" << err.message();
    emit error(err.message());
}

bool UDisks2MountDevice::mount()
{
    qDebug() << "MOUNT" << mPath.path();

    QDBusConnection c = QDBusConnection::systemBus();
    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.UDisks2",
                                                      mPath.path(),
                                                      "org.freedesktop.UDisks2.Filesystem",
                                                      "Mount");
    QVariantMap options;
    if (fileSystem() == "vfat")
        options.insert("options", "flush");

    msg << options;

    return c.callWithCallback(msg,
                              this,
                              SLOT(aboutToMount()),
                              SLOT(dbusError(QDBusError,QDBusMessage)));
}

bool UDisks2MountDevice::unmount()
{
    qDebug() << "UNMOUNT" << mPath.path();

    if (!mIsMounted)
        return true;

    QDBusConnection c = QDBusConnection::systemBus();
    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.UDisks2",
                                                      mPath.path(),
                                                      "org.freedesktop.UDisks2.Filesystem",
                                                      "Unmount");
    QVariantMap options;
    if (fileSystem() == "vfat")
        options.insert("options", "flush");

    msg << options;

    return c.callWithCallback(msg,
                              this,
                              SLOT(aboutToUnmount()),
                              SLOT(dbusError(QDBusError,QDBusMessage)));
}

bool UDisks2MountDevice::eject()
{
    qDebug() << "EJECT" << mPath.path();

    return mDriveIface->callWithCallback("Eject",
                                         QVariantList(),
                                         this,
                                         SLOT(aboutToEject()),
                                         SLOT(dbusError(QDBusError,QDBusMessage)));
}

void UDisks2MountDevice::aboutToMount()
{
    update();
    emit mounted();
}

void UDisks2MountDevice::aboutToUnmount()
{
    update();
    emit unmounted();
}

void UDisks2MountDevice::aboutToEject()
{
    qDebug() << "UDisks2MountDevice::aboutToEject success";
}
