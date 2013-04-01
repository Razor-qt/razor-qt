/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2013 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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

#ifndef GLOBAL_ACTION_DAEMON__DBUS_ACTION__INCLUDED
#define GLOBAL_ACTION_DAEMON__DBUS_ACTION__INCLUDED


#include "base_action.h"

#include <QString>
#include <QDBusObjectPath>
#include <QDBusConnection>


class ClientProxy;

class DBusAction : public BaseAction
{
public:
    DBusAction(LogTarget *logTarget, const QString &service, const QDBusObjectPath &path, const QString &description);
    DBusAction(LogTarget *logTarget, const QDBusConnection &connection, const QString &service, const QDBusObjectPath &path, const QString &description, bool persistent = false);
    ~DBusAction();

    static const char *id() { return "dbus"; }

    virtual const char *type() const { return id(); }

    virtual bool call();

    void shortcutChanged(const QString &oldShortcut, const QString &newShortcut);

    const QString &service() const { return mService; }
    const QDBusObjectPath &path() const { return mPath; }

    void appeared(const QDBusConnection &connection);
    void disappeared();

    bool isPresent() const { return mProxy; }

//    void makePersistent() { mPersistent = true; }
//    void makeTemporary() { mPersistent = false; }
    bool isPersistent() const { return mPersistent; }

private:
    ClientProxy *mProxy;

    QString mService;
    QDBusObjectPath mPath;

    bool mPersistent;
};

#endif // GLOBAL_ACTION_DAEMON__DBUS_ACTION__INCLUDED
