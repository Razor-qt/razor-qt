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

#ifndef NOTIFYADAPTOR_H
#define NOTIFYADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include "notifyd.h"
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;

/*
 * Adaptor class for interface org.freedesktop.Notifications
 */
class NotifyAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Notifications")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.freedesktop.Notifications\">\n"
"    <method name=\"GetServerInformation\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"name\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"vendor\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"version\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"spec_version\"/>\n"
"    </method>\n"
"    <method name=\"CloseNotification\">\n"
"      <arg direction=\"in\" type=\"u\" name=\"id\"/>\n"
"    </method>\n"
"    <method name=\"Notify\">\n"
"      <annotation value=\"QVariantMap\" name=\"com.trolltech.QtDBus.QtTypeName.In6\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"app_name\"/>\n"
"      <arg direction=\"in\" type=\"u\" name=\"replaces_id\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"app_icon\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"summary\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"body\"/>\n"
"      <arg direction=\"in\" type=\"as\" name=\"actions\"/>\n"
"      <arg direction=\"in\" type=\"a{sv}\" name=\"hints\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"expire_timeout\"/>\n"
"      <arg direction=\"out\" type=\"u\" name=\"id\"/>\n"
"    </method>\n"
"    <method name=\"GetCapabilities\">\n"
"      <arg direction=\"out\" type=\"as\" name=\"capabilities\"/>\n"
"    </method>\n"
"    <signal name=\"ActionInvoked\">\n"
"      <arg type=\"u\"/>\n"
"      <arg type=\"s\"/>\n"
"    </signal>\n"
"    <signal name=\"NotificationClosed\">\n"
"      <arg type=\"u\"/>\n"
"      <arg type=\"u\"/>\n"
"    </signal>\n"
"  </interface>\n"
        "")
public:
    NotifyAdaptor(Notifyd *parent);
    virtual ~NotifyAdaptor();

    inline Notifyd *parent() const
    { return static_cast<Notifyd *>(QObject::parent()); }

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void CloseNotification(uint id);
    QStringList GetCapabilities();
    QString GetServerInformation(QString &vendor, QString &version, QString &spec_version);
    uint Notify(const QString &app_name, uint replaces_id, const QString &app_icon, const QString &summary, const QString &body, const QStringList &actions, const QVariantMap &hints, int expire_timeout);
Q_SIGNALS: // SIGNALS
    void ActionInvoked(uint in0, const QString &in1);
    void NotificationClosed(uint in0, uint in1);
};

#endif
