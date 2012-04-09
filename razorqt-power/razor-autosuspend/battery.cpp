#include "battery.h"
#include <QDBusConnection>
#include <QDBusReply>
#include <QList>
#include <QDBusObjectPath>
#include <QDebug>

Battery::Battery()
{
    uPower = new QDBusInterface("org.freedesktop.UPower", "/org/freedesktop/UPower", "org.freedesktop.UPower", QDBusConnection::systemBus());
    QDBusReply<QList<QDBusObjectPath> > reply = uPower->call("EnumerateDevices");
    foreach (QDBusObjectPath objectPath, reply.value())
    {
        QDBusInterface *device = new QDBusInterface("org.freedesktop.UPower",
                                                    objectPath.path(),
                                                    "org.freedesktop.UPower.Device",
                                                    QDBusConnection::systemBus());

        if (device->property("Type").toUInt() == 2 && device->property("PowerSupply").toBool())
        {
            uPowerBatteryDevice = device;
            connect(uPowerBatteryDevice, SIGNAL(Changed()), this, SLOT(uPowerBatteryChanged()));
            break;
        }
        else
        {
            delete device;
        }
    }
}

Battery::~Battery()
{
}

void Battery::uPowerBatteryChanged()
{
    bool onBattery =  uPower->property("OnBattery").toBool();
    m_percentage = uPowerBatteryDevice->property("Percentage").toDouble();
    m_powerLow = m_percentage < POWER_LOW_LEVEL && onBattery;

    emit levelChanged(m_percentage);
}

bool Battery::powerLow()
{
    return m_powerLow;
}


