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

#ifndef GLOBAL_KEY_SHORTCUT_CLIENT__ACTION__INCLUDED
#define GLOBAL_KEY_SHORTCUT_CLIENT__ACTION__INCLUDED

#include <QtGlobal>

#include <QObject>
#include <QString>


namespace GlobalKeyShortcut
{

class ActionImpl;
class ClientImpl;

#ifndef SHARED_EXPORT
#define SHARED_EXPORT Q_DECL_IMPORT
#endif

class SHARED_EXPORT Action : public QObject
{
    Q_OBJECT

    friend class ActionImpl;
    friend class ClientImpl;

public:
    ~Action();

    QString changeShortcut(const QString &shortcut);
    bool changeDescription(const QString &description);

    QString path() const;
    QString shortcut() const;
    QString description() const;

    bool isValid() const;

signals:
    void activated();
    void shortcutChanged(const QString &oldShortcut, const QString &newShortcut);

private:
    Action(QObject *parent = 0);

    ActionImpl *impl;
};

}

#endif // GLOBAL_KEY_SHORTCUT_CLIENT__ACTION__INCLUDED
