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

#ifndef NOTIFYD_H
#define NOTIFYD_H

#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtCore/QVariantMap>
#include <QtCore/QStringList>

#include "notificationarea.h"

class RazorSettings;

/*
 * Class for interface org.freedesktop.Notifications
 */
class Notifyd: public QObject
{
    Q_OBJECT

public:
    explicit Notifyd(QObject* parent = 0);
    virtual ~Notifyd();

public slots:
    void CloseNotification(uint id);
    QStringList GetCapabilities();
    QString GetServerInformation(QString& vendor, QString& version, QString& spec_version);
    uint Notify(const QString& app_name,
                uint replaces_id,
                const QString& app_icon,
                const QString& summary,
                const QString& body,
                const QStringList& actions,
                const QVariantMap& hints,
                int expire_timeout);
signals:
    // signals for DBUS API specs
    void ActionInvoked(uint in0, const QString& in1);
    void NotificationClosed(uint in0, uint in1);

    // singals for our implementation
    void notificationClosed(uint id, uint reason);
    void notificationAdded(uint id, const QString &application, const QString &title,
                           const QString &description, const QString &icon,
                           int timeout, const QStringList& actions, const QVariantMap& hints);

private:
    uint mId;
    NotificationArea *m_area;
    RazorSettings *m_settings;
};

#endif // NOTIFYD_H
