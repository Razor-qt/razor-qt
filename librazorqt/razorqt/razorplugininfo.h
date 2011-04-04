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

#ifndef RAZORPLUGININFO_H
#define RAZORPLUGININFO_H

#include "xdgdesktopfile.h"
#include <QtCore/QString>
#include <QtCore/QList>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QtAlgorithms>
#include <QtCore/QDebug>

class QDir;
class QLibrary;

/*!
Every plugin needs a .desktop file that describes it. The basename of this file must
be same as the basename of the plugin library.

razorpanel_clock2.desktop file

[Desktop Entry]
  Type=Service
  ServiceTypes=RazorPanel/Plugin
  Name=Clock
  Comment=Clock and calendar

RazorPluginInfo class gives the interface for reading the values from the plugin .desktop file.
To make your own layout manager,
This is a pure virtual class, you must implement libraryDir(), translationDir(), and instance() methods.

*/
class RazorPluginInfo: public XdgDesktopFile
{
    Q_OBJECT

public:
    /*! Constructs a RazorPluginInfo object for accessing the info stored in the .desktop file
    called fileName. */
    explicit RazorPluginInfo(const QString& pluginDesktopFile, QObject *parent = 0);

    //! Reimplemented from XdgDesktopFile.
    RazorPluginInfo(const RazorPluginInfo& other, QObject *parent = 0);

    //! Reimplemented from XdgDesktopFile.
    RazorPluginInfo& operator=(const RazorPluginInfo& other);


    //! Returns identification string of this plugin. This Id used as a settigs section name.
    QString id() const { return mId; }

    //! This function is provided for convenience. It's equivalent to calling value("ServiceTypes").toString().
    QString serviceType() const  { return value("ServiceTypes").toString(); }

    //! Reimplemented from XdgDesktopFile.
    virtual bool isValid() const;

    /*! This is a factory method, it must create an instance of the plug-in.
    Example:
    RazorPanelPlugin* RazorPanelPluginInfo::instance(const QString& configFile, const QString& configSection, QObject* parent)
    {
        RazorPanel* panel = qobject_cast<RazorPanel*>(parent);

        if (!panel) return 0;

        QLibrary* lib = loadLibrary();
        if (!lib)
        {
            delete lib;
            return 0;
        }

        PluginInitFunction initFunc = (PluginInitFunction) lib->resolve("init");

        if (!initFunc) return 0;

        RazorPalelPluginStartInfo startInfo;
        startInfo.configFile = configFile;
        startInfo.configSection = configSection;
        startInfo.panel = panel;
        startInfo.pluginInfo = this;
        return initFunc(&startInfo, panel);
    }
    */
    virtual QObject* instance(const QString& configFile, const QString& configSection, QObject* parent = 0) = 0;

protected:
    //! Returns the directory where placed the plugin library file.
    virtual QString libraryDir() const = 0;

    //! Returns the directory where placed the plugin translations file.
    virtual QString translationDir() const = 0;

    //! Loads the library and returns QLibrary object if the library was loaded successfully; otherwise returns 0.
    QLibrary* loadLibrary();

private:
    QString mId;
};

//! List of the RazorPluginInfo objects.
template <typename TPluginInfo>
class RazorPluginInfoList: public QList<TPluginInfo*>
{
public:
    //! Constructs an empty list.
    explicit RazorPluginInfoList(): QList<TPluginInfo*>() {}

    //! Destroys the list and its RazorPluginInfo entries.
    virtual ~RazorPluginInfoList() { qDeleteAll(*this); }

    /*! Append RazorPluginInfo objects for the matched files in the directory.
      The list takes ownership of created objects.
      @param desktopFilesDir - scanned directory name.
      @param serviceType - type of the plugin, for example "RazorPanel/Plugin".
      @param nameFilter  - wildcard filter that understands * and ? wildcards. */
    void load(const QString& desktopFilesDir, const QString& serviceType, const QString& nameFilter="*")
    {
        QDir dir(desktopFilesDir);
        QFileInfoList files = dir.entryInfoList(QStringList(nameFilter), QDir::Files | QDir::Readable);
        foreach (QFileInfo file, files)
        {
            TPluginInfo* item = new TPluginInfo(file.canonicalFilePath());

            if (item->isValid() && item->serviceType() == serviceType)
                append(item);
            else
                delete item;
        }

    }

    //! Returns the first PluginInfo Id that matches the id parameter. Returns 0 if no item matched.
    TPluginInfo* const find(const QString& id) const
    {
        QListIterator<TPluginInfo*> it(*this);
        while (it.hasNext())
        {
            TPluginInfo* item = it.next();
            if (item->id() == id )
                return item;
        }

        return 0;
    }
};



QDebug operator<<(QDebug dbg, const RazorPluginInfo& pi);
QDebug operator<<(QDebug dbg, const RazorPluginInfo* const pi);


template <typename TPluginInfo>
QDebug operator<<(QDebug dbg, const RazorPluginInfoList<TPluginInfo>& list)
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


template <typename TPluginInfo>
QDebug operator<<(QDebug dbg, const RazorPluginInfoList<TPluginInfo>* const pluginInfoList)
{
    return operator<<(dbg, *pluginInfoList);
}


#endif // RAZORPLUGININFO_H
