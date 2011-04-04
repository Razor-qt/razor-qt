/********************************************************************
  Copyright: 2011 Alexander Sokoloff <sokoloff.a@gmail.ru>

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
    mId = QFileInfo(fileName()).completeBaseName().remove(QRegExp("^.*_"));
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
QLibrary* RazorPluginInfo::loadLibrary()
{
    QString baseName = QFileInfo(fileName()).completeBaseName();

    QString soPath = QString("%1/lib%2.so").arg(libraryDir(), baseName);
    QLibrary* library = new QLibrary(soPath);

    if (!library->load())
    {
        qWarning() << QString("Can't load plugin lib \"%1\"").arg(soPath) << library->errorString();
        delete library;
        return 0;
    }

    QString locale = QLocale::system().name();
    QTranslator* translator = new QTranslator(library);
    translator->load(QString("%1/%2_%3.qm").arg(translationDir(), baseName, locale));
    qApp->installTranslator(translator);

    return library;
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



