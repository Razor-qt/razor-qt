#ifndef RAZORDEVMAN_H
#define RAZORDEVMAN_H
#include "defs.h"

class RazorDev: public QObject
{
    Q_OBJECT
public:
    RazorDev ( QObject* parent = 0 );
    virtual bool mount();
    virtual bool umount();
    virtual bool getMounted();
    virtual QString getuuid();
    virtual QString getblockDev();
    virtual QString getvolume();
    virtual QString getdriveType();
    virtual QString getfsType();
    virtual QString getMountPoint();

};



class RazorDevMan: public QObject
{
    Q_OBJECT
public:
    RazorDevMan ( QObject* parent = 0 );
    virtual bool mount (QString _uuid);
    virtual bool umount (QString _uuid);
    virtual QList<QString> listDevices();
    virtual RazorDev* getDevice(QString _uuid);

signals:
    void deviceAdded ( QString );
    void deviceRemoved ( QString );


};




#endif
