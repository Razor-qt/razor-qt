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

#include "native_client_export.hpp"
#include "native_client_p.hpp"
#include "global_action_p.hpp"
#include "org.razorqt.global_action.native.h"

#include <QDBusConnection>



GlobalActionNativeClientImpl::GlobalActionNativeClientImpl(GlobalActionNativeClient *interface, QObject *parent)
    : QObject(parent)
    , mInterface(interface)
    , mDestructing(false)
    , mServiceWatcher(new QDBusServiceWatcher("org.razorqt.global_action", QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForOwnerChange, this))
    , mDaemonPresent(false)
{
    connect(mServiceWatcher, SIGNAL(serviceUnregistered(QString)), this, SLOT(daemonDisappeared(QString)));
    connect(mServiceWatcher, SIGNAL(serviceRegistered(QString)), this, SLOT(daemonAppeared(QString)));
    mProxy = new org::razorqt::global_action::native("org.razorqt.global_action", "/native", QDBusConnection::sessionBus(), this);
    mDaemonPresent = mProxy->isValid();

    connect(this, SIGNAL(emitShortcutGrabbed(QString)), mInterface, SIGNAL(shortcutGrabbed(QString)));
    connect(this, SIGNAL(emitGrabShortcutFailed()), mInterface, SIGNAL(grabShortcutFailed()));
    connect(this, SIGNAL(emitGrabShortcutCancelled()), mInterface, SIGNAL(grabShortcutCancelled()));
    connect(this, SIGNAL(emitGrabShortcutTimedout()), mInterface, SIGNAL(grabShortcutTimedout()));
    connect(this, SIGNAL(emitDaemonDisappeared()), mInterface, SIGNAL(daemonDisappeared()));
    connect(this, SIGNAL(emitDaemonAppeared()), mInterface, SIGNAL(daemonAppeared()));
    connect(this, SIGNAL(emitDaemonPresenceChanged(bool)), mInterface, SIGNAL(daemonPresenceChanged()));
}

GlobalActionNativeClientImpl::~GlobalActionNativeClientImpl()
{
    mDestructing = true;
}

void GlobalActionNativeClientImpl::daemonDisappeared(const QString &)
{
    mDaemonPresent = false;
    emit emitDaemonDisappeared();
    emit emitDaemonPresenceChanged(mDaemonPresent);
}

void GlobalActionNativeClientImpl::daemonAppeared(const QString &)
{
    foreach (QSharedPointer<GlobalAction> globalAction, mActions)
    {
        GlobalActionImpl *globalActionImpl = globalAction->impl;

        QDBusPendingReply<QString, qulonglong> reply = mProxy->addDBusAction(globalActionImpl->shortcut(), QDBusObjectPath(QString("/global_action/") + globalActionImpl->path()), globalActionImpl->description());
        reply.waitForFinished();
        globalActionImpl->setValid(!reply.isError() && reply.argumentAt<1>());

        if (globalActionImpl->isValid())
            globalActionImpl->setShortcut(reply.argumentAt<0>());
    }
    mDaemonPresent = true;
    emit emitDaemonAppeared();
    emit emitDaemonPresenceChanged(mDaemonPresent);
}

bool GlobalActionNativeClientImpl::isDaemonPresent() const
{
    return mDaemonPresent;
}

GlobalAction* GlobalActionNativeClientImpl::addDBusAction(const QString &shortcut, const QString &path, const QString &description)
{
    if (!QRegExp("[A-Za-z0-9_]+").exactMatch(path))
        return 0;

    if (mActions.contains(path))
        return 0;

    QDBusObjectPath dBusPath(QString("/global_action/") + path);

    QSharedPointer<GlobalAction> globalAction(new GlobalAction(this));

    GlobalActionImpl *globalActionImpl = new GlobalActionImpl(this, globalAction.data(), path, description, this);
    globalAction->impl = globalActionImpl;

    if (!QDBusConnection::sessionBus().registerObject(dBusPath.path(), globalActionImpl))
        return 0;

    if (mDaemonPresent)
    {
        QDBusPendingReply<QString, qulonglong> reply = mProxy->addDBusAction(shortcut, dBusPath, description);
        reply.waitForFinished();
        globalActionImpl->setValid(!reply.isError() && reply.argumentAt<1>());
        if (globalActionImpl->isValid())
            globalActionImpl->setShortcut(reply.argumentAt<0>());
    }
    else
        globalActionImpl->setValid(false);

    mActions[path] = globalAction;


    return globalAction.data();
}

QString GlobalActionNativeClientImpl::changeDBusShortcut(const QString &path, const QString &shortcut)
{
    if (!mActions.contains(path))
        return QString();

    QDBusPendingReply<QString> reply = mProxy->changeDBusShortcut(QDBusObjectPath(QString("/global_action/") + path), shortcut);
    reply.waitForFinished();
    if (reply.isError())
        return QString();

    return reply.argumentAt<0>();
}

bool GlobalActionNativeClientImpl::modifyDBusAction(const QString &path, const QString &description)
{
    if (!mActions.contains(path))
        return false;

    QDBusPendingReply<bool> reply = mProxy->modifyDBusAction(QDBusObjectPath(QString("/global_action/") + path), description);
    reply.waitForFinished();
    if (reply.isError())
        return false;

    return reply.argumentAt<0>();
}

bool GlobalActionNativeClientImpl::removeDBusAction(const QString &path)
{
    if (mDestructing)
        return true;

    if (!mActions.contains(path))
        return false;

    QDBusPendingReply<bool> reply = mProxy->removeDBusAction(QDBusObjectPath(QString("/global_action/") + path));
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

void GlobalActionNativeClientImpl::cancelShortutGrab()
{
    mProxy->cancelShortcutGrab();
}

void GlobalActionNativeClientImpl::grabShortcutFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QString, bool, bool, bool> reply = *call;
    if (reply.isError())
        emit emitGrabShortcutFailed();
    else
    {
        if (reply.argumentAt<1>())
            emit emitGrabShortcutFailed();
        else
        {
            if (reply.argumentAt<2>())
                emit emitGrabShortcutCancelled();
            else
            {
                if (reply.argumentAt<3>())
                    emit emitGrabShortcutTimedout();
                else
                    emit emitShortcutGrabbed(reply.argumentAt<0>());
            }
        }
    }

    call->deleteLater();
}


static GlobalActionNativeClient *globalActionNativeClient = 0;

GlobalActionNativeClient* GlobalActionNativeClient::instance()
{
    if (!globalActionNativeClient)
        globalActionNativeClient = new GlobalActionNativeClient();

    return globalActionNativeClient;
}

GlobalActionNativeClient::GlobalActionNativeClient()
    : QObject(0)
    , impl(new GlobalActionNativeClientImpl(this, this))
{
}

GlobalActionNativeClient::~GlobalActionNativeClient()
{
    delete impl;
    globalActionNativeClient = 0;
}

GlobalAction* GlobalActionNativeClient::addAction(const QString &shortcut, const QString &path, const QString &description) { return impl->addDBusAction(shortcut, path, description); }
void GlobalActionNativeClient::grabShortcut(uint timeout) { impl->grabShortcut(timeout); }
void GlobalActionNativeClient::cancelShortutGrab() { impl->cancelShortutGrab(); }
bool GlobalActionNativeClient::isDaemonPresent() const { return impl->isDaemonPresent(); }
