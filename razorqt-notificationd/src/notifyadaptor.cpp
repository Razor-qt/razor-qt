/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Alec Moskvin <alecm@gmx.com>
 *   Petr Vanek <petr@scribus.info>
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


#include "notifyadaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class NotifyAdaptor
 */

NotifyAdaptor::NotifyAdaptor(Notifyd *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

NotifyAdaptor::~NotifyAdaptor()
{
    // destructor
}

void NotifyAdaptor::CloseNotification(uint id)
{
    // handle method call org.freedesktop.Notifications.CloseNotification
    parent()->CloseNotification(id);
}

QStringList NotifyAdaptor::GetCapabilities()
{
    // handle method call org.freedesktop.Notifications.GetCapabilities
    return parent()->GetCapabilities();
}

QString NotifyAdaptor::GetServerInformation(QString &vendor, QString &version, QString &spec_version)
{
    // handle method call org.freedesktop.Notifications.GetServerInformation
    return parent()->GetServerInformation(vendor, version, spec_version);
}

uint NotifyAdaptor::Notify(const QString &app_name, uint replaces_id, const QString &app_icon, const QString &summary, const QString &body, const QStringList &actions, const QVariantMap &hints, int expire_timeout)
{
    // handle method call org.freedesktop.Notifications.Notify
    return parent()->Notify(app_name, replaces_id, app_icon, summary, body, actions, hints, expire_timeout);
}
