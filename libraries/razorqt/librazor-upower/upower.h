/*
 * (c)GPL3
This is probably temporary code probably. We will merge it into potential shared library.

This code is based on cool Brieuc ROBLIN's pyrotools.org applets, license GPLv3.
*/

#ifndef DBUS_H
#define DBUS_H

#include "battery.h"

class QDBusInterface;
class QDBusServiceWatcher;


class UPower : public QObject{
    Q_OBJECT
    Q_CLASSINFO( "author", "Brieuc ROBLIN")
    Q_CLASSINFO( "url", "http://pt.pyrotools.org/")

public:
    UPower();
    ~UPower();

    void suspend();
    void hibernate();
    void reboot();
    void halt();

    bool canSuspend();
    bool canHibernate();
    bool canHalt();
    bool canReboot();

    const QString version() const;

private:
    QDBusInterface      *m_interfaceProps;
    QDBusInterface      *m_interface;
    QDBusServiceWatcher *m_watcher;

public Q_SLOTS:
    void probeDevices();
    bool connectInterfaces();

private Q_SLOTS:
    void deviceAdded(QString path );
    void changed();

Q_SIGNALS:
    void upowerAvailable();
    void upowerUnavailable();

    void batteryDetected(Battery *);
    void batteryDisconnected(QString);
    void onLowBattery();
    void onBattery();
    void onPowerSupply();

    void suspendFail();
    void hibernateFail();
    void suspendSucceed();
    void hibernateSucceed();
};

#endif // DBUS_H
