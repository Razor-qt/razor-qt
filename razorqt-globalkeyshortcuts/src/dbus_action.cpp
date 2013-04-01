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

#include "dbus_action.hpp"
#include "client_proxy.hpp"
#include "log_target.hpp"


DBusAction::DBusAction(LogTarget *logTarget, const QString &service, const QDBusObjectPath &path, const QString &description)
    : BaseAction(logTarget, description)
    , mProxy(0)
    , mService(service)
    , mPath(path)
    , mPersistent(true)
{
}

DBusAction::DBusAction(LogTarget *logTarget, const QDBusConnection &connection, const QString &service, const QDBusObjectPath &path, const QString &description, bool persistent)
    : BaseAction(logTarget, description)
    , mProxy(new ClientProxy(service, path, connection))
    , mService(service)
    , mPath(path)
    , mPersistent(persistent)
{
}

DBusAction::~DBusAction()
{
    delete mProxy;
}

bool DBusAction::call()
{
    if (!isEnabled())
    {
        return false;
    }

    if (!mProxy)
    {
        mLogTarget->log(LOG_WARNING, "No native client: \"%s\"", qPrintable(mService));
        return false;
    }

    mProxy->emitActivated();

    return true;
}

void DBusAction::appeared(const QDBusConnection &connection)
{
    if (mProxy) // should never happen
    {
        return;
    }
    mProxy = new ClientProxy(mService, mPath, connection);
}

void DBusAction::disappeared()
{
    delete mProxy;
    mProxy = 0;
}

void DBusAction::shortcutChanged(const QString &oldShortcut, const QString &newShortcut)
{
    if (mProxy)
    {
        mProxy->emitShortcutChanged(oldShortcut, newShortcut);
    }
}
