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

#include "daemon_adaptor.hpp"

#include "org.razorqt.global_action.daemon.h"


DaemonAdaptor::DaemonAdaptor(QObject * parent)
    : QObject(parent)
    , QDBusContext()
{
    new OrgRazorqtGlobalActionDaemonAdaptor(this);
}

QString DaemonAdaptor::addDBusAction(const QString &shortcut, const QDBusObjectPath &path, const QString &description, qulonglong &id)
{
    QPair<QString, qulonglong> result;
    emit onAddDBusAction(result, shortcut, path, description, calledFromDBus() ? message().service() : QString());
    QString usedShortcut = result.first;
    id = result.second;
    if (id)
        emit actionAdded(id);
    return usedShortcut;
}

QString DaemonAdaptor::addMethodAction(const QString &shortcut, const QString &service, const QDBusObjectPath &path, const QString &interface, const QString &method, const QString &description, qulonglong &id)
{
    QPair<QString, qulonglong> result;
    emit onAddMethodAction(result, shortcut, service, path, interface, method, description);
    QString usedShortcut = result.first;
    id = result.second;
    if (id)
        emit actionAdded(id);
    return usedShortcut;
}

QString DaemonAdaptor::addCommandAction(const QString &shortcut, const QString &command, const QStringList &arguments, const QString &description, qulonglong &id)
{
    QPair<QString, qulonglong> result;
    emit onAddCommandAction(result, shortcut, command, arguments, description);
    QString usedShortcut = result.first;
    id = result.second;
    if (id)
        emit actionAdded(id);
    return usedShortcut;
}

bool DaemonAdaptor::modifyDBusAction(const QDBusObjectPath &path, const QString &description)
{
    qulonglong id;
    emit onModifyDBusAction(id, path, description, calledFromDBus() ? message().service() : QString());
    if (id)
        emit actionModified(id);
    return id;
}

bool DaemonAdaptor::modifyActionDescription(qulonglong id, const QString &description)
{
    bool result;
    emit onModifyActionDescription(result, id, description);
    if (result)
        emit actionModified(id);
    return result;
}

bool DaemonAdaptor::modifyMethodAction(qulonglong id, const QString &service, const QDBusObjectPath &path, const QString &interface, const QString &method, const QString &description)
{
    bool result;
    emit onModifyMethodAction(result, id, service, path, interface, method, description);
    if (result)
        emit actionModified(id);
    return result;
}

bool DaemonAdaptor::modifyCommandAction(qulonglong id, const QString &command, const QStringList &arguments, const QString &description)
{
    bool result;
    emit onModifyCommandAction(result, id, command, arguments, description);
    if (result)
        emit actionModified(id);
    return result;
}

QString DaemonAdaptor::changeDBusShortcut(const QDBusObjectPath &path, const QString &shortcut)
{
    QPair<QString, qulonglong> result;
    emit onChangeDBusShortcut(result, path, shortcut, calledFromDBus() ? message().service() : QString());
    if (!result.first.isEmpty())
        emit actionShortcutChanged(result.second);
    return result.first;
}

QString DaemonAdaptor::changeShortcut(qulonglong id, const QString &shortcut)
{
    QString result;
    emit onChangeShortcut(result, id, shortcut);
    if (!result.isEmpty())
        emit actionShortcutChanged(id);
    return result;
}

bool DaemonAdaptor::swapActions(qulonglong id1, qulonglong id2)
{
    bool result;
    emit onSwapActions(result, id1, id2);
    if (result)
        emit actionsSwapped(id1, id2);
    return result;
}

bool DaemonAdaptor::removeDBusAction(const QDBusObjectPath &path)
{
    qulonglong id;
    emit onRemoveDBusAction(id, path, calledFromDBus() ? message().service() : QString());
    if (id)
        emit actionRemoved(id);
    return id;
}

bool DaemonAdaptor::removeAction(qulonglong id)
{
    bool result;
    emit onRemoveAction(result, id);
    if (result)
        emit actionRemoved(id);
    return result;
}

bool DaemonAdaptor::setMultipleActionsBehaviour(uint behaviour)
{
    if (behaviour >= MULTIPLE_ACTIONS_BEHAVIOUR__COUNT)
        return false;
    emit onSetMultipleActionsBehaviour(static_cast<MultipleActionsBehaviour>(behaviour));
    emit multipleActionsBehaviourChanged(behaviour);
    return true;
}

uint DaemonAdaptor::getMultipleActionsBehaviour()
{
    MultipleActionsBehaviour result;
    emit onGetMultipleActionsBehaviour(result);
    return result;
}

QList<qulonglong> DaemonAdaptor::getAllActionIds()
{
    QList<qulonglong> result;
    emit onGetAllActionIds(result);
    return result;
}

bool DaemonAdaptor::getActionById(qulonglong id, QString &shortcut, QString &type, QString &info, QString &description)
{
    QPair<bool, GeneralActionInfo> result;
    emit onGetActionById(result, id);
    bool success = result.first;
    if (success)
    {
        shortcut = result.second.shortcut;
        type = result.second.type;
        info = result.second.info;
        description = result.second.description;
    }
    return success;
}

QMap<qulonglong,GeneralActionInfo> DaemonAdaptor::getAllActionsById()
{
    QMap<qulonglong,GeneralActionInfo> result;
    emit onGetAllActionsById(result);
    return result;
}

bool DaemonAdaptor::getDBusActionInfoById(qulonglong id, QString &service, QDBusObjectPath &path)
{
    QPair<bool, DBusActionInfo> result;
    emit onGetDBusActionInfoById(result, id);
    bool success = result.first;
    if (success)
    {
        service = result.second.service;
        path = result.second.path;
    }
    return success;
}

bool DaemonAdaptor::getMethodActionInfoById(qulonglong id, QString &service, QDBusObjectPath &path, QString &interface, QString &method)
{
    QPair<bool, MethodActionInfo> result;
    emit onGetMethodActionInfoById(result, id);
    bool success = result.first;
    if (success)
    {
        service = result.second.service;
        path = result.second.path;
        interface = result.second.interface;
        method = result.second.method;
    }
    return success;
}

bool DaemonAdaptor::getCommandActionInfoById(qulonglong id, QString &command, QStringList &arguments)
{
    QPair<bool, CommandActionInfo> result;
    emit onGetCommandActionInfoById(result, id);
    bool success = result.first;
    if (success)
    {
        command = result.second.command;
        arguments = result.second.arguments;
    }
    return success;
}

QString DaemonAdaptor::grabShortcut(uint timeout, bool &failed, bool &cancelled, bool &timedout)
{
    QString shortcut;
    emit onGrabShortcut(timeout, shortcut, failed, cancelled, timedout, message());
    return shortcut;
}

void DaemonAdaptor::quit()
{
    emit onQuit();
}
