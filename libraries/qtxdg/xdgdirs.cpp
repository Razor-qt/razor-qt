/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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


#include "xdgdirs.h"
#include <stdlib.h>
#include <QtCore/QDir>
#include <QtCore/QDebug>


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
QString xdgSingleDir(const QString &envVar, const QString &def, bool createDir)
{
    QString s(getenv(envVar.toAscii()));

    if (!s.isEmpty())
        fixBashShortcuts(s);
    else
        s = QString("%1/%2").arg(getenv("HOME"), def);

    QDir d(s);
    if (createDir && !d.exists())
    {
        if (!d.mkpath("."))
            qWarning() << QString("Can't create %1 directory.").arg(d.absolutePath());
    }

    return d.absolutePath();
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
QString XdgDirs::dataHome(bool createDir)
{
    return xdgSingleDir("XDG_DATA_HOME", ".local/share", createDir);
}


/************************************************

 ************************************************/
QString XdgDirs::configHome(bool createDir)
{
    return xdgSingleDir("XDG_CONFIG_HOME", ".config", createDir);
}


/************************************************

 ************************************************/
QStringList XdgDirs::dataDirs()
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
QStringList XdgDirs::configDirs()
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
QString XdgDirs::cacheHome(bool createDir)
{
    return xdgSingleDir("XDG_CACHE_HOME", ".cache", createDir);
}


/************************************************

 ************************************************/
QString XdgDirs::runtimeDir()
{
    QString result(getenv("XDG_RUNTIME_DIR"));
    fixBashShortcuts(result);
    return result;
}


/************************************************

 ************************************************/
QString XdgDirs::autostartHome(bool createDir)
{
    QDir dir(QString("%1/autostart").arg(configHome(createDir)));

    if (createDir && !dir.exists())
    {
        if (!dir.mkpath("."))
            qWarning() << QString("Can't create %1 directory.").arg(dir.absolutePath());
    }

    return dir.absolutePath();
}


/************************************************

 ************************************************/
QStringList XdgDirs::autostartDirs()
{
    QStringList dirs;
    foreach(QString dir, configDirs())
        dirs << QString("%1/autostart").arg(dir);

    return dirs;
}
