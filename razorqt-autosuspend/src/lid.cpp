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
#include "lid.h"
#include <QDBusConnection>
#include <QDBusReply>

Lid::Lid()
{
    mUPowerInterface = new QDBusInterface("org.freedesktop.UPower", "/org/freedesktop/UPower", "org.freedesktop.UPower",
                                QDBusConnection::systemBus(), this);
    connect(mUPowerInterface, SIGNAL(Changed()), this, SLOT(uPowerChange()));
    mUPowerPropertiesInterface = new QDBusInterface("org.freedesktop.UPower", "/org/freedesktop/UPower", "org.freedesktop.DBus.Properties",
                                          QDBusConnection::systemBus(), this);
    mIsClosed = mUPowerPropertiesInterface->property("LidIsClosed").toBool();
}

void Lid::uPowerChange()
{
    bool newIsClosed = mUPowerInterface->property("LidIsClosed").toBool();
    if (newIsClosed != mIsClosed)
    {
        mIsClosed = newIsClosed;
        emit changed(mIsClosed);
    }
}
