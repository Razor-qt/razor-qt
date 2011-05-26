/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * END_COMMON_COPYRIGHT_HEADER */


#ifndef XDGICON_H
#define XDGICON_H

#include <QIcon>
#include <QHash>
#include <QString>
#include <QStringList>

class XdgIcon
{
public:
    static QIcon const fromTheme(const QString& iconName, const QIcon& fallback = QIcon());
    static QIcon const fromTheme(const QStringList& iconNames, const QIcon& fallback = QIcon());

    static QString themeName();
    static void setThemeName(const QString& themeName);

    static QIcon const defaultApplicationIcon();

protected:
    explicit XdgIcon();
    virtual ~XdgIcon();
private:

};

#endif // XDGICON_H
