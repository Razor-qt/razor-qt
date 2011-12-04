/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
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
RazorPluginInfo::RazorPluginInfo(const QString& pluginDesktopFile, QObject *parent):
    XdgDesktopFile(pluginDesktopFile, parent)
{
    mId = QFileInfo(fileName()).completeBaseName();
}


/************************************************

 ************************************************/
RazorPluginInfo::RazorPluginInfo(const RazorPluginInfo& other, QObject *parent):
    XdgDesktopFile(other, parent)
{
     operator=(other);
}


/************************************************

 ************************************************/
RazorPluginInfo& RazorPluginInfo::operator=(const RazorPluginInfo& other)
{
    XdgDesktopFile::operator=(other);
    mId = other.mId;
    return *this;
}


/************************************************

 ************************************************/
bool RazorPluginInfo::isValid() const
{
    return XdgDesktopFile::isValid();// &&
           //(value("Type").toString() == "Service");

}


/************************************************

 ************************************************/
QLibrary* RazorPluginInfo::loadLibrary(const QString& libDir) const
{
    QString baseName, path;
    {
        QFileInfo fi = QFileInfo(fileName());
        baseName = fi.completeBaseName();
        path = fi.canonicalPath();
    }

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
RazorPluginInfoList::RazorPluginInfoList():
    QList<RazorPluginInfo*>()
{
}


/************************************************

 ************************************************/
RazorPluginInfoList::~RazorPluginInfoList()
{
    qDeleteAll(*this);
}


/************************************************

 ************************************************/
void RazorPluginInfoList::load(const QString& desktopFilesDir, const QString& serviceType, const QString& nameFilter)
{
    QDir dir(desktopFilesDir);
    QFileInfoList files = dir.entryInfoList(QStringList(nameFilter), QDir::Files | QDir::Readable);
    foreach (QFileInfo file, files)
    {
        RazorPluginInfo* item = new RazorPluginInfo(file.canonicalFilePath());

        if (item->isValid() && item->serviceType() == serviceType)
            append(item);
        else
            delete item;
    }

}


/************************************************

 ************************************************/
RazorPluginInfo* const RazorPluginInfoList::find(const QString& id) const
{
    QListIterator<RazorPluginInfo*> it(*this);
    while (it.hasNext())
    {
        RazorPluginInfo* item = it.next();
        if (item->id() == id )
            return item;
    }

    return 0;
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
