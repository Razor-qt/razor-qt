#ifndef RAZORDEVMAN_H
#define RAZORDEVMAN_H
#include "defs.h"

class Razordev: public QObject
{
    Q_OBJECT
public:
    Razordev ( QObject* parent = 0 );
    virtual bool mount();
    virtual bool umount();
    virtual bool getMounted();
    virtual QString getuuid();
    virtual QString getblockDev();
    virtual QString getvolume();
    virtual QString getdriveType();
    virtual QString getfsType();
    
};



class Razordevman: public QObject
{
    Q_OBJECT
public:
    Razordevman ( QObject* parent = 0 );
  virtual bool mount (QString _uuid);
  virtual bool umount (QString _uuid);
  virtual QList<QString> listDevices();
  virtual Razordev* getDevice(QString _uuid);

signals:
    void deviceAdded ( QString );
    void deviceRemoved ( QString );


};




#endif
