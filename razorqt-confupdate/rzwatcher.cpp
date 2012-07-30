/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
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


#include "rzwatcher.h"
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QDebug>
#include <QtCore/QDir>

RzWatcher::RzWatcher(bool debug, QObject *parent) :
    QObject(parent),
    m_debug(debug)
{
    connect(&m_updWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(updDirChanged(QString)));
    connect(&m_parentDirWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(parentDirChanged(QString)));
    addDirs();
}

void RzWatcher::addDirs()
{
    QStringList dirs = RzUpdate::updDirs();
    foreach (QString dirName, dirs)
    {
        QDir dir(dirName);

        if (dir.exists())
        {
            m_updWatcher.addPath(dir.absolutePath());
        }
        else
        {
            QString p = dir.absolutePath() + "/..";
            while (!dir.cd(p))
                p += "/..";

            m_parentDirWatcher.addPath(dir.absolutePath());
        }
    }

}

void RzWatcher::updDirChanged(const QString &path)
{
    RzUpdate rzUpdate;
    rzUpdate.setDebug(m_debug);
    rzUpdate.run();
}

void RzWatcher::parentDirChanged(const QString &path)
{
    m_parentDirWatcher.removePath(path);
    addDirs();
}
