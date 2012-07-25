/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Christian Surlykke <christian@surlykke.dk>
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
#include "battery.h"
#include <QDBusConnection>
#include <QDBusReply>
#include <QList>
#include <QDBusObjectPath>
#include <QDebug>
#include <QMetaObject>
#include <QMetaProperty>
#include <razorqt/razornotification.h>
#include <razorqt/razorsettings.h>
#include "../config/constants.h"

Battery::Battery()
{
    uPower = new QDBusInterface("org.freedesktop.UPower", "/org/freedesktop/UPower", "org.freedesktop.UPower", QDBusConnection::systemBus());
    uPowerBatteryDevice = 0;
    QDBusReply<QList<QDBusObjectPath> > reply = uPower->call("EnumerateDevices");
    foreach (QDBusObjectPath objectPath, reply.value())
    {
        QDBusInterface *device = new QDBusInterface("org.freedesktop.UPower",
                                                    objectPath.path(),
                                                    "org.freedesktop.UPower.Device",
                                                    QDBusConnection::systemBus());

        if (device->property("Type").toUInt() == 2 &&
            ( device->property("PowerSupply").toBool() ||                          // UPower < 0.9.16.3 wrongly reports this false for some laptop batteries
              device->property("NativePath").toString().contains("power_supply"))) // - hence this line
        {
            uPowerBatteryDevice = device;
            connect(uPowerBatteryDevice, SIGNAL(Changed()), this, SLOT(uPowerBatteryChanged()));
            uPowerBatteryProperties = new QDBusInterface("org.freedesktop.UPower",
                                                         objectPath.path(),
                                                         "org.freedesktop.DBus.Properties",
                                                         QDBusConnection::systemBus());
            uPowerBatteryChanged();
            break;
        }
        else
        {
            delete device;
        }
    }
    if (uPowerBatteryDevice == 0)
    {
        RazorNotification::notify(tr("No battery!"),
                                  tr("Razor autosuspend could not find data about any battery - actions on power low will not work"),
                                  "razor-autosuspend");
    }
}

Battery::~Battery()
{
}

void Battery::uPowerBatteryChanged()
{
    m_onBattery =  uPower->property("OnBattery").toBool();
    m_chargeLevel = uPowerBatteryDevice->property("Percentage").toDouble();

    QDBusReply<QVariantMap> reply = uPowerBatteryProperties->call("GetAll", "org.freedesktop.UPower.Device");
    props = reply.value();
    qDebug() << "props:" << properties();
    qDebug() << properties().size();

    emit batteryChanged();
}


double Battery::chargeLevel()
{
    return m_chargeLevel;
}


bool Battery::powerLow()
{
    return  m_onBattery && m_chargeLevel <  RazorSettings("razor-autosuspend").value(POWERLOWLEVEL_KEY, 15).toInt();
}

bool Battery::onBattery()
{
    return m_onBattery;
}

QVariantMap Battery::properties()
{
    return props;
}
