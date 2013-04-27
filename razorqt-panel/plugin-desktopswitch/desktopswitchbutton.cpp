/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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


#include <QToolButton>
#include <QtDebug>
#include <razor-global-key-shortcuts-client/razor-global-key-shortcuts-client.h>

#include "desktopswitchbutton.h"

DesktopSwitchButton::DesktopSwitchButton(QWidget * parent, int index, const QString &path, const QString &shortcut, const QString &title)
    : QToolButton(parent)
    , m_shortcut(0)
    , mIndex(index)
{
    setText(QString::number(index + 1));
    setCheckable(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    if (!shortcut.isEmpty())
    {
        QString description = tr("Switch to desktop %1").arg(index + 1);
        if (!title.isEmpty())
        {
            description.append(QString(" (%1)").arg(title));
        }
        m_shortcut = GlobalKeyShortcut::Client::instance()->addAction(QString(), path, description, this);
        if (m_shortcut)
        {
            if (m_shortcut->shortcut().isEmpty())
                m_shortcut->changeShortcut(shortcut);
            connect(m_shortcut, SIGNAL(activated()), this, SIGNAL(activated()));
        }
    }
    
    if (!title.isEmpty())
    {
        setToolTip(title);
    }
}

void DesktopSwitchButton::unregisterShortcut()
{
    GlobalKeyShortcut::Client::instance()->removeAction(QString("/desktop_switch/desktop_%1").arg(mIndex + 1));
}
