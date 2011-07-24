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

#include "xdgenv.h"


/************************************************
 Helper func.
 ************************************************/
void fixBashShortcuts(QString &s)
{
    if (s.startsWith('~'))
        s = QString(getenv("HOME")) + (s).mid(1);
}


/************************************************
 Helper func.
 ************************************************/
QString xdgSingleDir(const QString &envVar, const QString &def)
{
    QString result(getenv(envVar.toAscii()));
    if (!result.isEmpty())
    {
        fixBashShortcuts(result);
        return result;
    }

    return QString("%1/%2").arg(getenv("HOME"), def);
}


/************************************************
 Helper func.
 ************************************************/
QStringList xdgDirList(const QString &envVar)
{
    QStringList dirs = QString(getenv(envVar.toAscii())).split(':', QString::SkipEmptyParts);
    for (QStringList::Iterator i=dirs.begin(); i!=dirs.end(); ++i)
    {
        fixBashShortcuts((*i));
    }
    return dirs;
}


/************************************************

 ************************************************/
QString XdgEnv::dataHome()
{
    return xdgSingleDir("XDG_DATA_HOME", ".local/share");
}


/************************************************

 ************************************************/
QString XdgEnv::configHome()
{
    return xdgSingleDir("XDG_CONFIG_HOME", ".config");
}


/************************************************

 ************************************************/
QStringList XdgEnv::dataDirs()
{
    QStringList dirs = xdgDirList("XDG_DATA_DIRS");
    if (dirs.isEmpty())
    {
        dirs << "/usr/local/share/";
        dirs << "/usr/share/";
    }

    return dirs;
}


/************************************************

 ************************************************/
QStringList XdgEnv::configDirs()
{
    QStringList dirs = xdgDirList("XDG_CONFIG_DIRS");
    if (dirs.isEmpty())
    {
        dirs << "/etc/xdg";
    }

    return dirs;
}


/************************************************

 ************************************************/
QString XdgEnv::cacheHome()
{
    return xdgSingleDir("XDG_CACHE_HOME", ".cache");
}


/************************************************

 ************************************************/
QString XdgEnv::runtimeDir()
{
    QString result(getenv("XDG_RUNTIME_DIR"));
    fixBashShortcuts(result);
    return result;
}
