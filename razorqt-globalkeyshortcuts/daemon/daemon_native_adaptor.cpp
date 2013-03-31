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

#include "daemon_native_adaptor.hpp"

#include "org.razorqt.global_action.daemon_native.h"


DaemonNativeAdaptor::DaemonNativeAdaptor(QObject * parent)
    : QObject(parent)
    , QDBusContext()
{
    new OrgRazorqtGlobalActionDaemonNativeAdaptor(this);
}

QString DaemonNativeAdaptor::addDBusAction(const QString &shortcut, const QDBusObjectPath &path, const QString &description, qulonglong &id)
{
    QPair<QString, qulonglong> result;
    emit onAddDBusAction(result, shortcut, path, description, calledFromDBus() ? message().service() : QString());
    QString usedShortcut = result.first;
    id = result.second;
    return usedShortcut;
}

bool DaemonNativeAdaptor::modifyDBusAction(const QDBusObjectPath &path, const QString &description)
{
    qulonglong result;
    emit onModifyDBusAction(result, path, description, calledFromDBus() ? message().service() : QString());
    return result;
}

QString DaemonNativeAdaptor::changeDBusShortcut(const QDBusObjectPath &path, const QString &shortcut)
{
    QPair<QString, qulonglong> result;
    emit onChangeDBusShortcut(result, path, shortcut, calledFromDBus() ? message().service() : QString());
    QString usedShortcut = result.first;
    return usedShortcut;
}

bool DaemonNativeAdaptor::removeDBusAction(const QDBusObjectPath &path)
{
    qulonglong result;
    emit onRemoveDBusAction(result, path, calledFromDBus() ? message().service() : QString());
    return result;
}
