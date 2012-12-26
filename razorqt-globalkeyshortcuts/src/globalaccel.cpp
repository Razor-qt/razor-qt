/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Aaron Lewis <the.warl0ck.1989@gmail.com>
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

#include <QtDBus/QDBusConnection>
#include <QtCore/QProcess>
#include <QtCore/QDebug>
#include "globalaccel.h"

AbstractShortcut::AbstractShortcut(const QKeySequence& sequence, QObject* parent) :
    mShortcut(sequence, this)
{
    connect(&mShortcut, SIGNAL(activated()), SLOT(exec()));
}

CommandShortcut::CommandShortcut(const QKeySequence& sequence, const QString& cmd, QObject* parent):
    AbstractShortcut(sequence, parent),
    m_cmd (cmd)
{
}

bool CommandShortcut::exec()
{
    return QProcess::startDetached (m_cmd);
}

////
DBusShortcut::DBusShortcut( const QKeySequence& sequence,
                            const QString & dest,
                            const QString & path,
                            const QString & interface,
                            const QString & method,
                            const QList<QVariant> & parameters,
                            QObject *parent) :
    AbstractShortcut(sequence, parent),
    mMessage(QDBusMessage::createMethodCall(dest, path, interface, method))
{
    mMessage.setArguments(parameters);
}

bool DBusShortcut::exec()
{
    return QDBusConnection::sessionBus().send(mMessage);
}

////////////////////

GlobalAccel::GlobalAccel(QObject *parent):
    QObject (parent),
    m_shortcutSettings (new RazorSettings("globalkeyshortcuts", this))
{
    // read settings
    rebindAll();

    // monitor changes
    connect(m_shortcutSettings , SIGNAL(settingsChanged())  , SLOT(rebindAll()));
}

GlobalAccel::~GlobalAccel()
{
    qDeleteAll(mMapping);
    delete m_shortcutSettings;
}

void GlobalAccel::rebindAll()
{
    qDebug() << "Loading settings...";
    // clear existing
    qDeleteAll(mMapping);
    mMapping.clear();

    foreach(const QString & group , m_shortcutSettings->childGroups())
    {
        m_shortcutSettings->beginGroup(group);

        // shortcut is not enabled
        if (!m_shortcutSettings->value("Enabled").toBool())
        {
            continue;
        }

        QKeySequence sequence(group);
        const QString& type = m_shortcutSettings->value("Type").toString();
        // if shortcut is dbus call
        if (type.toLower() == "dbus")
        {
            mMapping.append(new DBusShortcut(sequence,
                        m_shortcutSettings->value("Destination").toString(),
                        m_shortcutSettings->value("Path").toString(),
                        m_shortcutSettings->value("Interface").toString(),
                        m_shortcutSettings->value("Method").toString(),
                        m_shortcutSettings->value("Parameter").toList()
                        ));
        }
        // otherwise it "should" be a command execution for now
        else
        {
            const QString& cmd = m_shortcutSettings->value("Exec").toString();
            // command is not empty
            if (!cmd.isEmpty())
            {
                mMapping.append(new CommandShortcut(sequence, cmd));
            }
        }

        m_shortcutSettings->endGroup();
    }
}
