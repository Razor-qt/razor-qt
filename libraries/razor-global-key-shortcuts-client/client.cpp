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

#include "client.h"
#include "client_p.h"
#include "action_p.h"
#include "org.razorqt.global_key_shortcuts.native.h"

#include <QDBusConnection>


namespace GlobalKeyShortcut
{

ClientImpl::ClientImpl(Client *interface, QObject *parent)
    : QObject(parent)
    , mInterface(interface)
    , mServiceWatcher(new QDBusServiceWatcher("org.razorqt.global_key_shortcuts", QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForOwnerChange, this))
    , mDaemonPresent(false)
{
    connect(mServiceWatcher, SIGNAL(serviceUnregistered(QString)), this, SLOT(daemonDisappeared(QString)));
    connect(mServiceWatcher, SIGNAL(serviceRegistered(QString)), this, SLOT(daemonAppeared(QString)));
    mProxy = new org::razorqt::global_key_shortcuts::native("org.razorqt.global_key_shortcuts", "/native", QDBusConnection::sessionBus(), this);
    mDaemonPresent = mProxy->isValid();

    connect(this, SIGNAL(emitShortcutGrabbed(QString)), mInterface, SIGNAL(shortcutGrabbed(QString)));
    connect(this, SIGNAL(emitGrabShortcutFailed()), mInterface, SIGNAL(grabShortcutFailed()));
    connect(this, SIGNAL(emitGrabShortcutCancelled()), mInterface, SIGNAL(grabShortcutCancelled()));
    connect(this, SIGNAL(emitGrabShortcutTimedout()), mInterface, SIGNAL(grabShortcutTimedout()));
    connect(this, SIGNAL(emitDaemonDisappeared()), mInterface, SIGNAL(daemonDisappeared()));
    connect(this, SIGNAL(emitDaemonAppeared()), mInterface, SIGNAL(daemonAppeared()));
    connect(this, SIGNAL(emitDaemonPresenceChanged(bool)), mInterface, SIGNAL(daemonPresenceChanged(bool)));
}

ClientImpl::~ClientImpl()
{
    QMap<QString, Action*>::iterator M = mActions.end();
    for (QMap<QString, Action*>::iterator I = mActions.begin(); I != M; ++I)
    {
        QDBusConnection::sessionBus().unregisterObject(QString("/global_key_shortcuts") + I.key());

        delete I.value();
    }
    mActions.clear();
}

void ClientImpl::daemonDisappeared(const QString &)
{
    mDaemonPresent = false;
    emit emitDaemonDisappeared();
    emit emitDaemonPresenceChanged(mDaemonPresent);
}

void ClientImpl::daemonAppeared(const QString &)
{
    QMap<QString, Action*>::iterator last = mActions.end();
    for (QMap<QString, Action*>::iterator I = mActions.begin(); I != last; ++I)
    {
        ActionImpl *globalActionImpl = I.value()->impl;

        QDBusPendingReply<QString, qulonglong> reply = mProxy->addClientAction(globalActionImpl->shortcut(), QDBusObjectPath(globalActionImpl->path()), globalActionImpl->description());
        reply.waitForFinished();
        globalActionImpl->setValid(!reply.isError() && reply.argumentAt<1>());

        if (globalActionImpl->isValid())
        {
            globalActionImpl->setShortcut(reply.argumentAt<0>());
        }
    }
    mDaemonPresent = true;
    emit emitDaemonAppeared();
    emit emitDaemonPresenceChanged(mDaemonPresent);
}

bool ClientImpl::isDaemonPresent() const
{
    return mDaemonPresent;
}

Action *ClientImpl::addClientAction(const QString &shortcut, const QString &path, const QString &description, QObject *parent)
{
    if (!QRegExp("(/[A-Za-z0-9_]+){2,}").exactMatch(path))
    {
        return 0;
    }

    if (mActions.contains(path))
    {
        return 0;
    }

    Action *globalAction = new Action(parent);

    ActionImpl *globalActionImpl = new ActionImpl(this, globalAction, path, description, globalAction);
    globalAction->impl = globalActionImpl;

    if (!QDBusConnection::sessionBus().registerObject(QString("/global_key_shortcuts") + path, globalActionImpl))
    {
        return 0;
    }

    if (mDaemonPresent)
    {
        QDBusPendingReply<QString, qulonglong> reply = mProxy->addClientAction(shortcut, QDBusObjectPath(path), description);
        reply.waitForFinished();
        globalActionImpl->setValid(!reply.isError() && reply.argumentAt<1>());
        if (globalActionImpl->isValid())
        {
            globalActionImpl->setShortcut(reply.argumentAt<0>());
        }
    }
    else
    {
        globalActionImpl->setValid(false);
    }

    mActions[path] = globalAction;


    return globalAction;
}

void removeAction(Action *action);

QString ClientImpl::changeClientActionShortcut(const QString &path, const QString &shortcut)
{
    if (!mActions.contains(path))
    {
        return QString();
    }

    QDBusPendingReply<QString> reply = mProxy->changeClientActionShortcut(QDBusObjectPath(path), shortcut);
    reply.waitForFinished();
    if (reply.isError())
    {
        return QString();
    }

    return reply.argumentAt<0>();
}

bool ClientImpl::modifyClientAction(const QString &path, const QString &description)
{
    if (!mActions.contains(path))
    {
        return false;
    }

    QDBusPendingReply<bool> reply = mProxy->modifyClientAction(QDBusObjectPath(path), description);
    reply.waitForFinished();
    if (reply.isError())
    {
        return false;
    }

    return reply.argumentAt<0>();
}

bool ClientImpl::removeClientAction(const QString &path)
{
    if (!mActions.contains(path))
    {
        return false;
    }

    QDBusPendingReply<bool> reply = mProxy->removeClientAction(QDBusObjectPath(path));
    reply.waitForFinished();
    if (reply.isError())
    {
        return false;
    }

    QDBusConnection::sessionBus().unregisterObject(QString("/global_key_shortcuts") + path);

    mActions[path]->disconnect();
    mActions.remove(path);

    return reply.argumentAt<0>();
}

void ClientImpl::removeAction(ActionImpl *actionImpl)
{
    QString path = actionImpl->path();
    if (!mActions.contains(path))
    {
        return;
    }

    QDBusPendingReply<bool> reply = mProxy->deactivateClientAction(QDBusObjectPath(path));
    reply.waitForFinished();

    QDBusConnection::sessionBus().unregisterObject(QString("/global_key_shortcuts") + path);

    mActions[path]->disconnect();
    mActions.remove(path);
}

void ClientImpl::grabShortcut(uint timeout)
{
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(mProxy->grabShortcut(timeout), this);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)), this, SLOT(grabShortcutFinished(QDBusPendingCallWatcher *)));
}

void ClientImpl::cancelShorcutGrab()
{
    mProxy->cancelShortcutGrab();
}

void ClientImpl::grabShortcutFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QString, bool, bool, bool> reply = *call;
    if (reply.isError())
    {
        emit emitGrabShortcutFailed();
    }
    else
    {
        if (reply.argumentAt<1>())
        {
            emit emitGrabShortcutFailed();
        }
        else
        {
            if (reply.argumentAt<2>())
            {
                emit emitGrabShortcutCancelled();
            }
            else
            {
                if (reply.argumentAt<3>())
                {
                    emit emitGrabShortcutTimedout();
                }
                else
                {
                    emit emitShortcutGrabbed(reply.argumentAt<0>());
                }
            }
        }
    }

    call->deleteLater();
}


static Client *globalActionNativeClient = 0;

Client *Client::instance()
{
    if (!globalActionNativeClient)
    {
        globalActionNativeClient = new Client();
    }

    return globalActionNativeClient;
}

Client::Client()
    : QObject(0)
    , impl(new ClientImpl(this, this))
{
}

Client::~Client()
{
    globalActionNativeClient = 0;
}

Action *Client::addAction(const QString &shortcut, const QString &path, const QString &description, QObject *parent) { return impl->addClientAction(shortcut, path, description, parent); }
bool Client::removeAction(const QString &path) { return impl->removeClientAction(path); }
void Client::grabShortcut(uint timeout) { impl->grabShortcut(timeout); }
void Client::cancelShorcutGrab() { impl->cancelShorcutGrab(); }
bool Client::isDaemonPresent() const { return impl->isDaemonPresent(); }

}
