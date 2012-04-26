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


