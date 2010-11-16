#ifndef RAZOR_HAL_H
#define RAZOR_HAL_H
#include "defs.h"
#include "razordevman.h"

/**
 * @file razorhal.h
 * @brief declares class Razorhal, Razordev
 * @author Christopher "VdoP" Regali
 */



/**
 * @brief this is just a little struct-like class for holding our data
 */
class RazorHalDev : public RazorDev
{
    Q_OBJECT
public:
    RazorHalDev ( QString _uuid, QString _bDev, QString _volume, QString _driveType, QString _fsType );
    RazorHalDev ( QString _uuid );
    bool mount();
    bool umount();


    QString getuuid()
    {
        return uuid;
    }
    QString getblockDev()
    {
        return blockDev;
    }
    QString getvolume()
    {
        return volume;
    }
    QString getdriveType()
    {
        return driveType;
    }
    QString getfsType()
    {
        return fsType;
    }
    bool getMounted()
    {
        return mounted;
    }
    bool isUseful()
    {
        return useful;
    }
private:
    bool useful;
    bool mounted;
    QString uuid, blockDev, volume, driveType, fsType, mntDir;

};

/**
 * @brief manages the device mounting
 */

class RazorHal : public RazorDevMan
{
    Q_OBJECT
public:
    RazorHal ( QObject* parent = 0 );
    ~RazorHal();
    bool mount ( QString _uuid );
    bool umount ( QString _uuid );
    QList<QString> listDevices();
    RazorDev* getDevice ( QString _uuid )
    {
        return deviceList.value ( _uuid );
    }
private:
    QMap<QString, RazorHalDev*> deviceList;
    QDBusInterface* notifyDev;

private slots:
    void addDevice ( QString );
    void removeDevice ( QString );



};



#endif
