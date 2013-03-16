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

#ifndef GLOBAL_ACTION_NATIVE_CLIENT__NATIVE_CLIENT__IMPL__INCLUDED
#define GLOBAL_ACTION_NATIVE_CLIENT__NATIVE_CLIENT__IMPL__INCLUDED

#include <QObject>
#include <QString>
#include <QMap>
#include <QSharedPointer>

#include "global_action_export.hpp"

#include "org.razorqt.global_action.native.h"


class GlobalActionNativeClient;
class ClientAdaptor;
class QDBusServiceWatcher;

class GlobalActionNativeClientImpl : public QObject
{
Q_OBJECT

public:
    GlobalActionNativeClientImpl(GlobalActionNativeClient *interface, QObject *parent = 0);
    ~GlobalActionNativeClientImpl();

    GlobalAction* addDBusAction(const QString &shortcut, const QString &path, const QString &description);

    QString changeDBusShortcut(const QString &path, const QString &shortcut);
    bool modifyDBusAction(const QString &path, const QString &description);
    bool removeDBusAction(const QString &path);

    void grabShortcut(uint timeout);
    void cancelShortutGrab();

    bool isDaemonPresent() const;

public slots:
    void grabShortcutFinished(QDBusPendingCallWatcher *call);
    void daemonDisappeared(const QString &);
    void daemonAppeared(const QString &);

private:
    GlobalActionNativeClient *mInterface;
    org::razorqt::global_action::native *mProxy;
    QMap<QString, QSharedPointer<GlobalAction> > mActions;
    bool mDestructing;
    QDBusServiceWatcher *mServiceWatcher;
    bool mDaemonPresent;
};

#endif // GLOBAL_ACTION_NATIVE_CLIENT__NATIVE_CLIENT__IMPL__INCLUDED
