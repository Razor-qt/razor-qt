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


#ifndef DESKTOPSWITCHBUTTON_H
#define DESKTOPSWITCHBUTTON_H

#include <QtGui/QToolButton>


namespace GlobalKeyShortcut
{
class Action;
}

class DesktopSwitchButton : public QToolButton
{
    Q_OBJECT
    
public:
    DesktopSwitchButton(QWidget * parent, int index, const QString &path, const QString &shortcut, const QString &title=QString());

public slots:
    void unregisterShortcut();

signals:
    void activated();

private:
    GlobalKeyShortcut::Action * m_shortcut;
    int mIndex;
};

#endif
