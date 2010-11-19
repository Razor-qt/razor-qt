#ifndef RAZOR_HAL_CPP
#define RAZOR_HAL_CPP
#include "razorhal.h"

/**
 * @file razorhal.h
 * @brief implements class Razorhal, Razordev
 * @author Christopher "VdoP" Regali
 */


/**
 * @brief simple constructor where everything is set by hand - try not to use it
 */
RazorHalDev::RazorHalDev ( QString _uuid, QString _bDev, QString _volume, QString _driveType, QString _fsType )
{
    uuid = _uuid;
    blockDev = _bDev;
    volume = _volume;
    driveType = _driveType;
    fsType = _fsType;
}

/**
 * @brief this gets the current mountpoint of the device and returns it
 */
QString RazorHalDev::getMountPoint()
{
    QDBusInterface uuid_interface ( "org.freedesktop.Hal", uuid, "org.freedesktop.Hal.Device", QDBusConnection::systemBus(), this );
    QDBusReply<QString> mnt_point = uuid_interface.call ( "GetProperty", "volume.mount_point" );
    
    if (mnt_point.isValid() && !mnt_point.value().isEmpty())
    {
      mntDir = mnt_point.value();
      qDebug() << "mounted in: " << mntDir;
    }
    else
    {
      qDebug() << "could not get mount-point: " << mnt_point.error();
      qDebug() << "setting it by hand to /media/" << volume;
      mntDir = "/media/"+volume;
    }
    return mntDir;
}



/**
 * @brief umounts the device if possible - returns false on fail
 */

bool RazorHalDev::umount()
{
    QStringList opts;
    QDBusInterface uuid_interface ( "org.freedesktop.Hal", uuid, "org.freedesktop.Hal.Device", QDBusConnection::systemBus(), this );
    QDBusInterface umount_interface ( "org.freedesktop.Hal", uuid, "org.freedesktop.Hal.Device.Volume",
                                      QDBusConnection::systemBus(), this );

    // unmount and (if CDROM) eject the device
    QDBusReply<int> umount_cmd = umount_interface.call ( "Eject", opts );

    if (umount_cmd.isValid())
    {
        mounted = false;
        return true;
    }
    else
    {
        qDebug() << "could not unmount: " << umount_cmd.error();
        return false;
    }
}

/**
 * @brief mounts the device if possible - returns false on fail
 */
bool RazorHalDev::mount()
{
    QStringList opts;
    qDebug() << "Razordev: trying to mount: " << uuid;
    QDBusInterface uuid_interface ( "org.freedesktop.Hal", uuid, "org.freedesktop.Hal.Device", QDBusConnection::systemBus(), this );
    QDBusInterface mount_interface ( "org.freedesktop.Hal", uuid, "org.freedesktop.Hal.Device.Volume",
                                     QDBusConnection::systemBus(), this );

    // mount the device in /media/xxx
    qDebug() << "Trying to mount: " << uuid << " with fstype: " << fsType << " and volume: " << volume;
    QDBusReply<int> mnt_cmd = mount_interface.call ( "Mount", volume, fsType, opts );

    // get the mount point
    QDBusReply<QString> mnt_point = uuid_interface.call ( "GetProperty", "volume.mount_point" );
    
    if (mnt_point.isValid() && !mnt_point.value().isEmpty())
    {
      mntDir = mnt_point.value();
      qDebug() << "mounted in: " << mntDir;
    }
    else
    {
      qDebug() << "could not get mount-point: " << mnt_point.error();
      qDebug() << "setting it by hand to /media/" << volume;
      mntDir = "/media/"+volume;
    }
    
    
    if ( mnt_cmd.isValid() )
    {
        mounted = true;
        return true;
    }
    else
    {
        qDebug() << "Mount failed: " << mnt_cmd.error();
        return false;
    }
}


/**
 * @brief this constructor gets its infos via hal from a given uuid.. use this
 */

RazorHalDev::RazorHalDev ( QString _uuid )
{
    useful = false;
    uuid=_uuid;
    mounted = false;
    qDebug() << "Razordev: getting stuff from HAL for dev: " << _uuid;

    QDBusInterface uuid_interface ( "org.freedesktop.Hal", uuid, "org.freedesktop.Hal.Device", QDBusConnection::systemBus(), this );
    QDBusReply<bool> is_volume = uuid_interface.call ( "GetProperty", "block.is_volume" );

    qDebug() << "Print no interesting device: " << uuid;



    // this is some serious copypasta from antico - antico source never was ideal but i think this should work
    if ( is_volume.isValid() && is_volume.value() == true )
    {
        QDBusReply<QString> fs_usage = uuid_interface.call ( "GetProperty", "volume.fsusage" );
        QDBusReply<bool> ignored = uuid_interface.call ( "GetProperty", "volume.ignore" );

        if ( fs_usage.value() == "filesystem" && !ignored.value() )
        {
            QDBusReply<QString> block_dev = uuid_interface.call ( "GetProperty", "block.device" );

            if ( block_dev.isValid() && !block_dev.value().isEmpty() )
                blockDev = block_dev;

            QDBusReply<QString> volume_lab = uuid_interface.call ( "GetProperty", "volume.label" );

            if ( volume_lab.isValid() && !volume_lab.value().isEmpty() )
                volume = volume_lab;
	    

            QDBusReply<QString> volume_fs_type = uuid_interface.call ( "GetProperty", "volume.fstype" );

            if ( volume_fs_type.isValid() && !volume_fs_type.value().isEmpty() )
                fsType = volume_fs_type.value();

            QDBusReply<QString> storage_device = uuid_interface.call ( "GetProperty", "block.storage_device" );
            QDBusInterface storage_interface ( "org.freedesktop.Hal", storage_device.value(), "org.freedesktop.Hal.Device",
                                               QDBusConnection::systemBus(), this );

            QDBusReply<QString> drv_type = storage_interface.call ( "GetProperty", "storage.drive_type" );

            if ( drv_type.isValid() && !drv_type.value().isEmpty() )
                driveType = drv_type.value();

            QDBusReply<QMap<QString, QVariant> > props = storage_interface.call ( "GetAllProperties" );
            QMapIterator<QString, QVariant> iter ( props.value() );

            qDebug() << "Razordev: debug output";
            while ( iter.hasNext() )
            {
                iter.next();
                qDebug() << "Razordev: Property:" << iter.key() << iter.value(); // print all properties
            }

            qDebug() << "Deskdev added. Block device:" << blockDev << "Volume label:" << volume << "Volume fstype:"
                     << fsType << "Drive type:" << driveType << "UUID:" << uuid;
            useful = true;
	    
	    
	    if (volume.isEmpty() && !props.value().value("storage.model").toString().isEmpty())
	      volume = props.value().value("storage.model").toString();
	    else if (volume.isEmpty() && !driveType.isEmpty() )
	      volume=driveType;

        }
    }
    else
    {
        useful = false;
    }
}



/**
 * @brief this is calles by the dbus-stuff when we get a new device
 */
void RazorHal::addDevice ( QString _uuid )
{
    qDebug() << "Razorhal: adding device: "<< _uuid;
    RazorHalDev* tmp = new RazorHalDev ( _uuid );
    if (tmp->isUseful())
    {
        deviceList[_uuid]=tmp;
        emit ( deviceAdded ( _uuid ) );
    }
    else
        delete tmp;
}

/**
 * @brief returns a uuid-list of plugged devices
 */
QList< QString > RazorHal::listDevices()
{
    return deviceList.keys();
}


/**
 * @brief mounts a drive with a given uuid - returns true on success
 */
bool RazorHal::mount ( QString _uuid )
{
    qDebug() << "Razorhal: trying mount... device "<< _uuid;
    if ( deviceList.contains ( _uuid ) )
        return deviceList.value ( _uuid )->mount();
    else
        qDebug() << "Razorhal: device "<< _uuid <<" not in list: "<<listDevices();
    return false;
}

/**
 * @brief our constructor
 */
RazorHal::RazorHal ( QObject* parent ) : RazorDevMan ( parent )
{
    qDebug() << "Razorhal: initializing...";
    //first we connect to the hal interface via DBus
    notifyDev = new QDBusInterface ( "org.freedesktop.Hal", "/org/freedesktop/Hal/Manager", "org.freedesktop.Hal.Manager", QDBusConnection::systemBus(), this );
    notifyDev->connection().connect ( "org.freedesktop.Hal", "/org/freedesktop/Hal/Manager", "org.freedesktop.Hal.Manager", "DeviceAdded", this, SLOT ( addDevice ( QString ) ) );
    notifyDev->connection().connect ( "org.freedesktop.Hal", "/org/freedesktop/Hal/Manager", "org.freedesktop.Hal.Manager", "DeviceRemoved", this, SLOT ( removeDevice ( QString ) ) );
}

/**
 * @brief unmounts a given uuid
 */
bool RazorHal::umount ( QString _uuid )
{

    qDebug() << "Razorhal: trying umount... device "<< _uuid;
    if ( deviceList.contains ( _uuid ) )
        return  deviceList.value ( _uuid )->umount();
    else
        qDebug() << "Razorhal: device "<< _uuid <<" not in list: "<<listDevices();

    return false;
}

/**
 * @brief our destructor
 */
RazorHal::~RazorHal()
{
    for ( int i=0; i < deviceList.values().count(); i++ )
        delete deviceList.values().at ( i );
    delete notifyDev;
}

/**
 * @brief this is called on device-removal
 */
void RazorHal::removeDevice ( QString _uuid )
{
    qDebug() << "Razorhal: removed Device: "<< _uuid;
    if ( deviceList.contains ( _uuid ) )
    {
        delete deviceList.value ( _uuid );
        deviceList.remove ( _uuid );
        emit ( deviceRemoved ( _uuid ) );
    }
}






#endif
