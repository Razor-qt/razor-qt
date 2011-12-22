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
    for (int i = 0; i < config_dirs.count(); i++)
        pathList.append(config_dirs.at(i)+"/autostart/");
    pathList.push_back(XdgDirs::configHome()+"/autostart/");

    qDebug() << "XdgAutoStart: beginning building list";

    for (int i=0; i< pathList.length(); i++)
    {
        addDirtoList(pathList.at(i));
    }
    qDebug() << "XdgAutoStart: found "<<m_list.count()<< " items for the autostart...";
    cleanList();
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
        XdgDesktopFile * f = new XdgDesktopFile();
        f->load(_dir+filelist.at(i));
        if (f->isShow())
            m_list.append(f);
        else
        {
            badNames.push_back(f->value("Name").toString()); //this is needed as std says all with the same name get removed by one entry with these flags too
            delete f;
        }
    }
}

/**
 *@brief puts some output to qDebug()
 */

void XdgAutoStart::debugAutostart()
{
    for (int i = 0; i < m_list.count(); i++)
    {
        XdgDesktopFile * tmp = m_list.at(i);
        qDebug() << tmp->value("Name").toString();
    }

}


/**
 * @brief destructor
 * @todo Do This!
 */
XdgAutoStart::~XdgAutoStart()
{
    foreach(XdgDesktopFile * f, m_list)
        delete f;
    m_list.clear();
}


/**
 * @brief does clean the list according to the XDG-Standards
 */
void XdgAutoStart::cleanList()
{
    qDebug() << "XdgAutoStart: cleaning house! (" << badNames.count() << " bad Names )";
    for (int i = 0; i < m_list.count(); i++)
    {
        XdgDesktopFile * tmp = m_list.at(i);
        if (badNames.contains(tmp->value("Name").toString()))
        {
            delete m_list.takeAt(i);
        }
    }
    qDebug() << "XdgAutoStart: after cleaning: " << m_list.count()<< " items left.";
}


/**
 * @brief constructor without parameters. XdgAutoStart will use its own XdgDirs!
 * Useful for using this class alone without a manager.
 */

XdgAutoStart::XdgAutoStart()
{
    qDebug() << "XdgAutoStart: initialising with on XdgDirs...";
    updateList();
}


/**
 * @brief returns a pointer to the list of Desktopfiles we stored
 */
QList< XdgDesktopFile* > XdgAutoStart::list()
{
    return m_list;
}
