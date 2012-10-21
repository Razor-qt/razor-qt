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

Battery::Battery(QObject* parent) 
   : QObject(parent),
     mUPowerBatteryPropertiesInterface(0),
     mChargeLevel(0.0),
     mDecharging(false)
{
    mUPowerInterface = new QDBusInterface("org.freedesktop.UPower", "/org/freedesktop/UPower", "org.freedesktop.UPower", QDBusConnection::systemBus());
    mUPowerBatteryDeviceInterface = 0;
    QDBusReply<QList<QDBusObjectPath> > reply = mUPowerInterface->call("EnumerateDevices");
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
            mUPowerBatteryDeviceInterface = device;
            connect(mUPowerBatteryDeviceInterface, SIGNAL(Changed()), this, SLOT(uPowerBatteryChanged()));
            mUPowerBatteryPropertiesInterface = new QDBusInterface("org.freedesktop.UPower",
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
    if (mUPowerBatteryDeviceInterface == 0)
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
    mDecharging =  mUPowerInterface->property("OnBattery").toBool();
    mChargeLevel = mUPowerBatteryDeviceInterface->property("Percentage").toDouble();

    if (mUPowerBatteryPropertiesInterface)
    {
        QDBusReply<QVariantMap> reply = mUPowerBatteryPropertiesInterface->call("GetAll", "org.freedesktop.UPower.Device");
        mProperties = reply.value();
        qDebug() << "props:" << properties();

        emit batteryChanged();
    }
    else
        qWarning() << "uPowerBatteryProperties has not been initialized";
}


double Battery::chargeLevel()
{
    return mChargeLevel;
}


bool Battery::powerLow()
{
    return  mDecharging && mChargeLevel <  RazorSettings("razor-autosuspend").value(POWERLOWLEVEL_KEY, 15).toInt();
}

bool Battery::decharging()
{
    return mDecharging;
}

uint Battery::state()
{
    return mState;
}

QString Battery::stateAsString()
{
    return mStateAsString;
}

QVariantMap Battery::properties()
{
    return mProperties;
}

QString Battery::state2string(uint state) {
    switch (state) 
    {
        case 1:  return  tr("Charging");
        case 2:  return  tr("Discharging");
        case 3:  return  tr("Empty");
        case 4:  return  tr("Fully charged");
        case 5:  return  tr("Pending charge");
        case 6:  return  tr("Pending discharge");
        default: return  tr("Unknown");
    }
}
