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

#ifndef GLOBAL_ACTION_MANAGER__ACTIONS__INCLUDED
#define GLOBAL_ACTION_MANAGER__ACTIONS__INCLUDED

#include <QObject>
#include <QtGlobal>
#include <QString>
#include <QList>
#include <QStringList>
#include <QMap>
#include <QPair>
#include <QDBusObjectPath>

#include "../daemon/meta_types.hpp"


class OrgRazorqtGlobal_actionDaemonInterface;
namespace org {
  namespace razorqt {
    namespace global_action {
      typedef ::OrgRazorqtGlobal_actionDaemonInterface daemon;
    }
  }
}

class QDBusServiceWatcher;

class Actions : public QObject
{
    Q_OBJECT
public:
    Actions(QObject *parent = 0);
    ~Actions();


    QList<qulonglong> allActionIds() const;
    QPair<bool, GeneralActionInfo> actionById(qulonglong id) const;

    QList<qulonglong> allDBusActionIds() const;
    QPair<bool, DBusActionInfo> dBusActionInfoById(qulonglong id) const;

    QList<qulonglong> allMethodActionIds() const;
    QPair<bool, MethodActionInfo> methodActionInfoById(qulonglong id) const;

    QList<qulonglong> allCommandActionIds() const;
    QPair<bool, CommandActionInfo> commandActionInfoById(qulonglong id) const;

    MultipleActionsBehaviour multipleActionsBehaviour() const;

signals:
    void daemonDisappeared();
    void daemonAppeared();

    void actionAdded(qulonglong id);
    void actionEnabled(qulonglong id, bool enabled);
    void actionModified(qulonglong id);
    void actionsSwapped(qulonglong id1, qulonglong id2);
    void actionRemoved(qulonglong id);
    void multipleActionsBehaviourChanged(MultipleActionsBehaviour behaviour);

private:
    void init();
    void clear();

    QList<qulonglong> getAllActionIds();

    bool getActionById(qulonglong id, QString &shortcut, QString &description, bool &enabled, QString &type, QString &info);
    QMap<qulonglong,GeneralActionInfo> getAllActionsById();

    bool getDBusActionInfoById(qulonglong id, QString &shortcut, QString &description, bool &enabled, QString &service, QDBusObjectPath &path);
    bool getMethodActionInfoById(qulonglong id, QString &shortcut, QString &description, bool &enabled, QString &service, QDBusObjectPath &path, QString &interface, QString &method);
    bool getCommandActionInfoById(qulonglong id, QString &shortcut, QString &description, bool &enabled, QString &command, QStringList &arguments);

    bool isActionEnabled(qulonglong id);

    uint getMultipleActionsBehaviour();

private slots:
    void on_daemonDisappeared(const QString &);
    void on_daemonAppeared(const QString &);

    void do_actionAdded(qulonglong id);
    void on_actionAdded(qulonglong id);
    void on_actionEnabled(qulonglong id, bool enabled);
    void on_actionModified(qulonglong id);
    void on_actionShortcutChanged(qulonglong id);
    void on_actionsSwapped(qulonglong id1, qulonglong id2);
    void do_actionRemoved(qulonglong id);
    void on_actionRemoved(qulonglong id);
    void on_multipleActionsBehaviourChanged(uint behaviour);

private:
    org::razorqt::global_action::daemon *mDaemonProxy;
    QDBusServiceWatcher *mServiceWatcher;

    typedef QMap<qulonglong, GeneralActionInfo> GeneralActionInfos;
    GeneralActionInfos mGeneralActionInfo;

    typedef QMap<qulonglong, DBusActionInfo> DBusActionInfos;
    DBusActionInfos mDBusActionInfo;

    typedef QMap<qulonglong, MethodActionInfo> MethodActionInfos;
    MethodActionInfos mMethodActionInfo;

    typedef QMap<qulonglong, CommandActionInfo> CommandActionInfos;
    CommandActionInfos mCommandActionInfo;

    MultipleActionsBehaviour mMultipleActionsBehaviour;
};

#endif // GLOBAL_ACTION_MANAGER__ACTIONS__INCLUDED
