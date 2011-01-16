/********************************************************************
  Copyright: 2010 Alexander Sokoloff <sokoloff.a@gmail.ru>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*********************************************************************/

#ifndef XDGICON_H
#define XDGICON_H

#include <QIcon>
#include <QHash>
#include <QString>
#include <QStringList>

class XdgIcon
{
public:
    static QIcon const fromTheme(const QString& iconName, int size, const QIcon& fallback = QIcon());
    static QIcon const fromTheme(const QStringList& iconNames, int size, const QIcon& fallback = QIcon());

    static QString themeName();
    static void setThemeName(const QString& themeName);


protected:
    explicit XdgIcon();
    virtual ~XdgIcon();
private:

};

#endif // XDGICON_H
