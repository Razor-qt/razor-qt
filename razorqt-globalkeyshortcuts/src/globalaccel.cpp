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
#include <razorqt/razorsettings.h>

GlobalAccel::GlobalAccel()
{
    RazorSettings *m_shortcutSettings = new RazorSettings("globalaccel", this);
    foreach(const QString & group , m_shortcutSettings->childGroups())
    {
        m_shortcutSettings->beginGroup(group);
        const QString &cmd = m_shortcutSettings->value("Exec").toString();
        bool enabled = m_shortcutSettings->value("Enabled").toBool();
        if (enabled && ! cmd.isEmpty())
        {
            mapping.insert(QKeySequence(group) , cmd);
        }
        m_shortcutSettings->endGroup();
    }
    connect(m_shortcutSettings , SIGNAL(settingsChanged())  , SLOT(rebindAll()));
}

void GlobalAccel::bindDefault()
{
    foreach(const QKeySequence & keySequence , mapping.keys())
    {
        qDebug() << "Binding: " << keySequence << mapping.value(keySequence);

        QxtGlobalShortcut *sc = new QxtGlobalShortcut(this);
        sc->setShortcut(keySequence);
        connect(sc , SIGNAL(activated()) , SLOT(launchApp()));
    }
}

void GlobalAccel::rebindAll()
{
    qDebug() << "Rebind .. ";
    mapping.clear();
    bindDefault();
}

void GlobalAccel::launchAppFromDBus()
{

}

void GlobalAccel::launchApp()
{
    QxtGlobalShortcut *sc = qobject_cast<QxtGlobalShortcut *>(sender());
    QProcess::startDetached(mapping.value(sc->shortcut()));
}
