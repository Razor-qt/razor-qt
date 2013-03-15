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

#include "global_action_export.hpp"
#include "global_action_p.hpp"
#include "native_client_p.hpp"

#include "org.razorqt.global_action.client.h"


GlobalActionImpl::GlobalActionImpl(GlobalActionNativeClientImpl *client, GlobalAction *interface, const QString &path, const QString &description, QObject *parent)
    : QObject(parent)
    , mClient(client)
    , mInterface(interface)
    , mPath(path)
    , mDescription(description)
{
    new OrgRazorqtGlobalActionClientAdaptor(this);

    connect(this, SIGNAL(activated()), mInterface, SIGNAL(activated()));
    connect(this, SIGNAL(shortcutChanged(QString,QString)), mInterface, SIGNAL(shortcutChanged(QString,QString)));
}

GlobalActionImpl::~GlobalActionImpl()
{
    mClient->removeDBusAction(mPath);
}

QString GlobalActionImpl::changeShortcut(const QString &shortcut)
{
    mShortcut = mClient->changeDBusShortcut(mPath, shortcut);
    return mShortcut;
}

bool GlobalActionImpl::changeDescription(const QString &description)
{
    bool result = mClient->modifyDBusAction(mPath, description);
    if (result)
        mDescription = description;
    return result;
}

void GlobalActionImpl::setShortcut(const QString &shortcut)
{
    mShortcut = shortcut;
}

QString GlobalActionImpl::shortcut() const
{
    return mShortcut;
}

QString GlobalActionImpl::description() const
{
    return mDescription;
}


GlobalAction::GlobalAction(QObject *parent)
    : QObject(parent)
    , impl(0)
{
}

GlobalAction::~GlobalAction()
{
    delete impl;
}


QString GlobalAction::changeShortcut(const QString &shortcut) { return impl->changeShortcut(shortcut); }
bool GlobalAction::changeDescription(const QString &description) { return impl->changeDescription(description); }
QString GlobalAction::shortcut() const { return impl->shortcut(); }
QString GlobalAction::description() const { return impl->description(); }
