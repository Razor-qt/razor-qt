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

#include "global_action_native_client_export.hpp"
#include "global_action_native_client_p.hpp"
#include "client_adaptor.hpp"


GlobalActionNativeClientImpl::GlobalActionNativeClientImpl(QObject *parent)
    : QObject(parent)
{
    mProxy = new org::razorqt::global_action::native("org.razorqt.global_action", "/native", QDBusConnection::sessionBus(), this);
}

GlobalActionNativeClientImpl::~GlobalActionNativeClientImpl()
{
}

QString GlobalActionNativeClientImpl::addDBusAction(const QString &shortcut, const QString &path, const QString &description)
{
    if (!QRegExp("[A-Za-z0-9_]+").exactMatch(path))
        return QString();

    if (mActions.contains(path))
        return QString();

    QSharedPointer<ClientAdaptor> clientAdaptor(new ClientAdaptor(path, this));
    if (!QDBusConnection::sessionBus().registerObject(path, clientAdaptor.data()))
        return QString();

    QDBusPendingReply<QString, qulonglong> reply = mProxy->addDBusAction(shortcut, QDBusObjectPath(QString("/global_action/") + path), description);
    reply.waitForFinished();
    if (reply.isError())
        return QString();

    if (!reply.argumentAt<1>())
        return QString();

    connect(clientAdaptor.data(), SIGNAL(on_activated(QString)), iface, SIGNAL(activated(QString)));
    connect(clientAdaptor.data(), SIGNAL(on_shortcutChanged(QString,QString,QString)), iface, SIGNAL(shortcutChanged(QString,QString,QString)));

    mActions[path] = clientAdaptor;

    return reply.argumentAt<0>();
}

QString GlobalActionNativeClientImpl::changeDBusShortcut(const QString &path, const QString &shortcut)
{
    if (!mActions.contains(path))
        return QString();

    QDBusPendingReply<QString> reply = mProxy->changeDBusShortcut(QDBusObjectPath(path), shortcut);
    reply.waitForFinished();
    if (reply.isError())
        return QString();

    return reply.argumentAt<0>();
}

bool GlobalActionNativeClientImpl::modifyDBusAction(const QString &path, const QString &description)
{
    if (!mActions.contains(path))
        return false;

    QDBusPendingReply<bool> reply = mProxy->modifyDBusAction(QDBusObjectPath(path), description);
    reply.waitForFinished();
    if (reply.isError())
        return false;

    return reply.argumentAt<0>();
}

bool GlobalActionNativeClientImpl::removeDBusAction(const QString &path)
{
    if (!mActions.contains(path))
        return false;

    QDBusPendingReply<bool> reply = mProxy->removeDBusAction(QDBusObjectPath(path));
    reply.waitForFinished();
    if (reply.isError())
        return false;

    mActions[path]->disconnect();
    mActions.remove(path);

    return reply.argumentAt<0>();
}

void GlobalActionNativeClientImpl::grabShortcut(uint timeout)
{
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(mProxy->grabShortcut(timeout), this);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(grabShortcutFinished(QDBusPendingCallWatcher*)));
}

void GlobalActionNativeClientImpl::grabShortcutFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QString, bool, bool, bool> reply = *call;
    if (reply.isError())
    {
        emit iface->grabShortcutFailed();
    }
    else
    {
        if (reply.argumentAt<1>())
            emit iface->grabShortcutFailed();
        else
        {
            if (reply.argumentAt<2>())
                emit iface->grabShortcutCancelled();
            else
            {
                if (reply.argumentAt<3>())
                    emit iface->grabShortcutTimedout();
                else
                    emit iface->shortcutGrabbed(reply.argumentAt<0>());
            }
        }
    }

    call->deleteLater();
}


GlobalActionNativeClient::GlobalActionNativeClient(QObject *parent)
    : QObject(parent)
    , impl(new GlobalActionNativeClientImpl(this))
{
}

GlobalActionNativeClient::~GlobalActionNativeClient()
{
    delete impl;
}

QString GlobalActionNativeClient::addDBusAction(const QString &shortcut, const QString &path, const QString &description) { return impl->addDBusAction(shortcut, path, description); }
QString GlobalActionNativeClient::changeDBusShortcut(const QString &path, const QString &shortcut) { return impl->changeDBusShortcut(path, shortcut); }
bool GlobalActionNativeClient::modifyDBusAction(const QString &path, const QString &description) { return impl->modifyDBusAction(path, description); }
bool GlobalActionNativeClient::removeDBusAction(const QString &path) { return impl->removeDBusAction(path); }
void GlobalActionNativeClient::grabShortcut(uint timeout) { impl->grabShortcut(timeout); }
