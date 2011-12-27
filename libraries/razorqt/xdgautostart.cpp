/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL3+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *   Chris "VdoP" Regali
 *   Alec Moskvin <alecm@gmx.com>
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


#include <QtCore/QtDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include "xdgautostart.h"
#include <qtxdg/xdgdirs.h>

/**
 * @file XdgAutoStart.cpp
 * @brief Implements the class XdgAutoStart
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  */


/**
 * @brief updates the list of autostart items
 * gets a list of paths and calls addDirtoList for each path!
 */
void XdgAutoStart::updateList()
{
    qDebug()<< "XdgAutoStart: getting configpaths";
    QStringList pathList;
    QStringList config_dirs = XdgDirs::configDirs();
    pathList.append(XdgDirs::configHome()+"/autostart/");
    for (int i = 0; i < config_dirs.count(); i++)
        pathList.append(config_dirs.at(i)+"/autostart/");

    qDebug() << "XdgAutoStart: beginning building list";

    for (int i=0; i< pathList.length(); i++)
    {
        addDirtoList(pathList.at(i));
    }
    qDebug() << "XdgAutoStart: found "<<mMap.count()<< " items for the autostart...";
}

/**
 * @brief adds a given dir to the autostartlist
 * @param[in] _dir the directory to search for files
 **/
void XdgAutoStart::addDirtoList(const QString & _dir)
{
    qDebug() << "XdgAutoStart: adding "<<_dir << " to list!";
    //this is probably small so no dir-iterator.. just list them and work the list!
    QDir directory(_dir);

    directory.setNameFilters(QStringList() << "*.desktop"); //filter the .desktop files only
    QStringList filelist = directory.entryList();
    for (int i =0; i < filelist.count(); i++)
    {
        QString fileName = filelist.at(i);

        // The file in the "most important directory" overrides all others of the same filename
        if (!mBadNames.contains(fileName) && !mMap.contains(fileName))
        {
            XdgDesktopFile * file = new XdgDesktopFile();
            file->load(_dir + fileName);
            if (file->isApplicable(mExcludeHidden))
            {
                mMap.insert(fileName, file);
            }
            else
            {
                mBadNames.insert(fileName);
                delete file;
            }
        }
    }
}

/**
 *@brief puts some output to qDebug()
 */
void XdgAutoStart::debugAutostart()
{
    foreach (QString file, mMap.keys())
    {
        XdgDesktopFile * tmp = mMap.value(file);
        qDebug() << tmp->value("Name").toString();
    }
}


/**
 * @brief destructor
 * @todo Do This!
 */
XdgAutoStart::~XdgAutoStart()
{
    foreach(XdgDesktopFile * f, mMap)
        delete f;
    mMap.clear();
}


/**
 * @brief constructor without parameters. XdgAutoStart will use its own XdgDirs!
 * Useful for using this class alone without a manager.
 */
XdgAutoStart::XdgAutoStart(bool excludeHidden)
{
    mExcludeHidden = excludeHidden;
    qDebug() << "XdgAutoStart: initialising with on XdgDirs...";
    updateList();
}


/**
 * @brief saves the specified file in the home autostart directory, modifying
 * or overriding the original entry.
 */
bool XdgAutoStart::saveAutoStartFile(XdgDesktopFile* file)
{
    QString filePath = file->fileName();
    QString fileName = QFileInfo(filePath).fileName();
    if (!mMap.contains(fileName))
    {
        mMap.insert(fileName, file);
    }
    else if (mMap.value(fileName)->fileName() != filePath)
    {
        delete mMap.take(fileName);
        mMap.insert(fileName, file);
    }
    return file->save(XdgDirs::configHome() + "/autostart/" + fileName);
}


/**
 * @brief returns a pointer to the list of Desktopfiles we stored
 */
QList< XdgDesktopFile* > XdgAutoStart::list()
{
    return mMap.values();
}

/**
 * @brief returns a mapping of filenames to the Desktopfiles stored
 */
QMap< QString, XdgDesktopFile* > XdgAutoStart::map()
{
    return mMap;
}
