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


#ifndef XDGENV_H
#define XDGENV_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <stdlib.h>

class XdgEnv
{
public:
    /************************************************
     *
     ************************************************/
    static
    QString configDirs()
    {
        QStringList dirs = QString(getenv("XDG_CONFIG_DIRS")).split(':', QString::SkipEmptyParts);
        if (!dirs.isEmpty())
        {
            for (QStringList::Iterator i=dirs.begin(); i!=dirs.end(); ++i)
            {
                if ((*i).startsWith('~'))
                    *i = QString(getenv("HOME")) + (*i).mid(1);
            }

            return dirs.join(":");
        }

        return "/etc/xdg";
    }


    /************************************************
     *
     ************************************************/
    static
    QString configHome()
    {
        QString result(getenv("XDG_CONFIG_HOME"));
        if (!result.isEmpty())
            return result;

        return QString("%1/.config").arg(getenv("HOME"));
    }


    /************************************************
     *
     ************************************************/
    static
    QString dataDirs()
    {
        QStringList dirs = QString(getenv("XDG_DATA_DIRS")).split(':', QString::SkipEmptyParts);
        if (!dirs.isEmpty())
        {
            for (QStringList::Iterator i=dirs.begin(); i!=dirs.end(); ++i)
            {
                if ((*i).startsWith('~'))
                    *i = QString(getenv("HOME")) + (*i).mid(1);
            }

            return dirs.join(":");
        }

        return "/usr/local/share/:/usr/share/";
    }

};

#endif // XDGENV_H
