/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL3+
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
 * version 3 of the License, or (at your option) any later version.
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


#include "razorplugininfo.h"
#include <QtCore/QObject>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QTranslator>
#include <QtCore/QCoreApplication>
#include <QLibrary>
#include <QDebug>


/************************************************

 ************************************************/
RazorPluginInfo::RazorPluginInfo():
    XdgDesktopFile()
{

}


/************************************************

 ************************************************/
bool RazorPluginInfo::load(const QString& fileName)
{
    XdgDesktopFile::load(fileName);
    mId = QFileInfo(fileName).completeBaseName();
    return isValid();
}


/************************************************

 ************************************************/
bool RazorPluginInfo::isValid() const
{
    return XdgDesktopFile::isValid();
}


/************************************************

 ************************************************/
QLibrary* RazorPluginInfo::loadLibrary(const QString& libDir) const
{
    QString baseName, path;
    QFileInfo fi = QFileInfo(fileName());
    path = fi.canonicalPath();
    baseName = value("X-Razor-Library", fi.completeBaseName()).toString();

    QString soPath = QString("%1/lib%2.so").arg(libDir, baseName);
    QLibrary* library = new QLibrary(soPath);

    if (!library->load())
    {
        qWarning() << QString("Can't load plugin lib \"%1\"").arg(soPath) << library->errorString();
        delete library;
        return 0;
    }

    QString locale = QLocale::system().name();
    QTranslator* translator = new QTranslator(library);

    translator->load(QString("%1/%2/%2_%3.qm").arg(path, baseName, locale));
    qApp->installTranslator(translator);

    return library;
}


/************************************************

 ************************************************/
RazorPluginInfoList RazorPluginInfo::search(const QString& desktopFilesDir, const QString& serviceType, const QString& nameFilter)
{
    QList<RazorPluginInfo> res;

    QDir dir(desktopFilesDir);
    QFileInfoList files = dir.entryInfoList(QStringList(nameFilter), QDir::Files | QDir::Readable);
    foreach (QFileInfo file, files)
    {
        RazorPluginInfo item;
        item.load(file.canonicalFilePath());

        if (item.isValid() && item.serviceType() == serviceType)
            res.append(item);
    }

    return res;
}




/************************************************

 ************************************************/
QDebug operator<<(QDebug dbg, const RazorPluginInfo &pluginInfo)
{
    dbg.nospace() << QString("%1").arg(pluginInfo.id());
    return dbg.space();
}


/************************************************

 ************************************************/
QDebug operator<<(QDebug dbg, const RazorPluginInfo * const pluginInfo)
{
    return operator<<(dbg, *pluginInfo);
}


/************************************************

 ************************************************/
QDebug operator<<(QDebug dbg, const RazorPluginInfoList& list)
{
    dbg.nospace() << '(';
    for (int i=0; i<list.size(); ++i)
    {
        if (i) dbg.nospace() << ", ";
        dbg << list.at(i);
    }
    dbg << ')';
    return dbg.space();
}


/************************************************

 ************************************************/
QDebug operator<<(QDebug dbg, const RazorPluginInfoList* const pluginInfoList)
{
    return operator<<(dbg, *pluginInfoList);
}
