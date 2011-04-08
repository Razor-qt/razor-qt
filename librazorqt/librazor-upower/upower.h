#ifndef DBUS_H
#define DBUS_H

#include <QtDBus>

#include "battery.h"

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

    const QString version() const;

private:
    bool canSuspend();
    bool canHibernate();

    QDBusInterface      *m_interfaceProps;
    QDBusInterface      *m_interface;
    QDBusServiceWatcher *m_watcher;

public Q_SLOTS:
    void probeDevices();
    bool connectInterfaces();

private Q_SLOTS:
    void deviceAdded( QString path );
    void changed();
    void serviceUnregistered( QString service );

Q_SIGNALS:
    void upowerAvailable();
    void upowerUnavailable();

    void changed( Battery *bat );

    void batteryDetected( Battery * );
    void batteryDisconnected( QString );
    void onLowBattery();
    void onBattery();
    void onPowerSupply();

    void suspendFail();
    void hibernateFail();
    void suspendSucceed();
    void hibernateSucceed();

    void monitoring( QString );
};

#endif // DBUS_H
