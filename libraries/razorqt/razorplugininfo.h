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


#ifndef RAZORPLUGININFO_H
#define RAZORPLUGININFO_H

#include <qtxdg/xdgdesktopfile.h>
#include <QtCore/QString>
#include <QtCore/QList>

#include <QtCore/QFileInfo>
//#include <QtCore/QDir>
#include <QtCore/QtAlgorithms>
#include <QtCore/QDebug>

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
This is a pure virtual class, you must implement libraryDir(), translationDir(), and instance() methods.
*/
class RazorPluginInfo: public XdgDesktopFile
{
public:
    /// Constructs a RazorPluginInfo object for accessing the info stored in the .desktop file.
    explicit RazorPluginInfo();

    //! Reimplemented from XdgDesktopFile.
    virtual bool load(const QString& fileName);


    //! Reimplemented from XdgDesktopFile.
    //RazorPluginInfo& operator=(const RazorPluginInfo& other);


    //! Returns identification string of this plugin, identified plugin type. Now id is part of the filename.
    QString id() const { return mId; }

    //! This function is provided for convenience. It's equivalent to calling value("ServiceTypes").toString().
    QString serviceType() const  { return value("ServiceTypes").toString(); }

    //! Reimplemented from XdgDesktopFile.
    virtual bool isValid() const;

    /*! Loads the library and returns QLibrary object if the library was loaded successfully; otherwise returns 0.
        @parm libDir directory where placed the plugin .so file. */
    QLibrary* loadLibrary(const QString& libDir) const;


    /*! Returns a list of RazorPluginInfo objects for the matched files in the directory.
      @param desktopFilesDir - scanned directory name.
      @param serviceType - type of the plugin, for example "RazorPanel/Plugin".
      @param nameFilter  - wildcard filter that understands * and ? wildcards. */
    static QList<RazorPluginInfo> search(const QString& desktopFilesDir, const QString& serviceType, const QString& nameFilter="*");
private:
    QString mId;
};

typedef QList<RazorPluginInfo> RazorPluginInfoList;


QDebug operator<<(QDebug dbg, const RazorPluginInfo& pi);
QDebug operator<<(QDebug dbg, const RazorPluginInfo* const pi);

QDebug operator<<(QDebug dbg, const RazorPluginInfoList& list);
QDebug operator<<(QDebug dbg, const RazorPluginInfoList* const pluginInfoList);

#endif // RAZORPLUGININFO_H
