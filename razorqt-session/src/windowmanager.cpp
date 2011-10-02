/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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
 
#include "windowmanager.h"

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>


QMap<QString,QString> availableWindowManagers()
{
    QMap<QString,QString> candidates;
    candidates["openbox"] =     QObject::tr("Openbox - light-weight window manager");
    candidates["kwin"] =        QObject::tr("KWin - window manager of the KDE Software Compilation");
    candidates["metacity"] =    QObject::tr("Metacity - window manager of the GNOME desktop environment");
    candidates["windowmaker"] = QObject::tr("Windowmaker - a classical lightweight window manager");
    candidates["e16"] =         QObject::tr("Enlightenement 16");
    candidates["fvwm2"] =       QObject::tr("Fvwm2 - a classical lightweight window manager");
    
    foreach (QString s, candidates.keys())
    {
        if (!findProgram(s))
            candidates.remove(s);
    }
    
    return candidates;
}

bool findProgram(const QString &program)
{
    QString path = qgetenv("PATH");
    foreach(QString dir, path.split(":"))
    {

        QFileInfo fi= QFileInfo(dir + QDir::separator() + program);
        if (fi.isExecutable() )
            return true;
    }
    return false;
}
