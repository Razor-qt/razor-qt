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

#ifndef GLOBAL_ACTION__META_TYPES__INCLUDED
#define GLOBAL_ACTION__META_TYPES__INCLUDED


#include <qglobal.h>
#include <QtDBus/QDBusMetaType>


typedef enum MultipleActionsBehaviour
{
    MULTIPLE_ACTIONS_BEHAVIOUR_FIRST = 0, // queue
    MULTIPLE_ACTIONS_BEHAVIOUR_LAST,      // stack
    MULTIPLE_ACTIONS_BEHAVIOUR_NONE,      // qtcreator style
    MULTIPLE_ACTIONS_BEHAVIOUR_ALL,       // permissive behaviour
    MULTIPLE_ACTIONS_BEHAVIOUR__COUNT
} MultipleActionsBehaviour;

typedef struct GeneralActionInfo {
    QString shortcut;
    QString type;
    QString info;
    QString description;
} GeneralActionInfo;

typedef struct DBusActionInfo {
    QString service;
    QDBusObjectPath path;
} DBusActionInfo;

typedef struct MethodActionInfo {
    QString service;
    QDBusObjectPath path;
    QString interface;
    QString method;
} MethodActionInfo;

typedef struct CommandActionInfo {
    QString command;
    QStringList arguments;
} CommandActionInfo;




Q_DECLARE_METATYPE(GeneralActionInfo)

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    Q_DECLARE_METATYPE(QList<qulonglong>)
    typedef QMap<qulonglong,GeneralActionInfo> QMap_qulonglong_GeneralActionInfo;
    Q_DECLARE_METATYPE(QMap_qulonglong_GeneralActionInfo)
#endif



inline QDBusArgument& operator << (QDBusArgument &argument, const GeneralActionInfo &generalActionInfo)
{
    argument.beginStructure();
    argument << generalActionInfo.shortcut << generalActionInfo.type << generalActionInfo.info << generalActionInfo.description;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument& operator >> (const QDBusArgument &argument, GeneralActionInfo &generalActionInfo)
{
    argument.beginStructure();
    argument >> generalActionInfo.shortcut >> generalActionInfo.type >> generalActionInfo.info >> generalActionInfo.description;
    argument.endStructure();
    return argument;
}

#endif // GLOBAL_ACTION_MANAGER__META_TYPES__INCLUDED

