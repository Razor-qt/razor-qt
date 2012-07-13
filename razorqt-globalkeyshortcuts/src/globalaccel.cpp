/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Aaron Lewis <the.warl0ck.1989@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */
#include "globalaccel.h"

CommandShortcut::CommandShortcut (const QString & cmd, QObject *parent):
    AbstractShortcut (parent),
    m_cmd (cmd)
{
}

bool CommandShortcut::exec()
{
    return QProcess::startDetached (m_cmd);
}

////
DBusShortcut::DBusShortcut (const QString & dest, 
                            const QString & path,
                            const QString & interface,
                            const QString & method,
                            const QList<QVariant> & parameters,
                            QObject *parent) :
    AbstractShortcut (parent),
    m_dest (dest),
    m_path (path),
    m_interface (interface),
    m_method (method),
    m_params (parameters)

{
}

bool DBusShortcut::exec()
{
    QDBusMessage message = QDBusMessage::createMethodCall( 
            m_dest, 
            m_path,
            m_interface,
            m_method
            );
    message.setArguments (m_params);

    return QDBusConnection::sessionBus().send (message);
}

////////////////////

GlobalAccel::GlobalAccel(QObject *parent):
    QObject (parent),
    m_shortcutSettings (new RazorSettings("globalkeyshortcuts", this))
{
    // read settings
    bindDefault ();

    // monitor changes
    connect(m_shortcutSettings , SIGNAL(settingsChanged())  , SLOT(rebindAll()));
}

GlobalAccel::~GlobalAccel()
{

}

void GlobalAccel::bindDefault()
{
    // clear existing
    mapping.clear();

    foreach(const QString & group , m_shortcutSettings->childGroups())
    {
        m_shortcutSettings->beginGroup(group);

        const QString & cmd = m_shortcutSettings->value("Exec").toString();
        bool enabled = m_shortcutSettings->value("Enabled").toBool();

        // bind shortcut only when it's enabled by user
        if (enabled && ! cmd.isEmpty())
        {
            mapping.insert(QKeySequence(group), new CommandShortcut (cmd));

            // create shortcut
            QxtGlobalShortcut *sc = new QxtGlobalShortcut(this);
            sc->setShortcut(QKeySequence (group));
            connect(sc , SIGNAL(activated()) , SLOT(launchApp()));
        }

        m_shortcutSettings->endGroup();
    }

}

void GlobalAccel::rebindAll()
{
    qDebug() << "Reloading settings..";
    bindDefault();
}

void GlobalAccel::launchApp()
{
    QxtGlobalShortcut *sc = qobject_cast<QxtGlobalShortcut *>(sender());
    mapping.value ( sc->shortcut() )->exec();
}
