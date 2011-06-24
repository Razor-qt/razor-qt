//2010 Chris "VdoP" Regali

#include <QtCore/QtDebug>
#include <QtCore/QDir>

#include "xdgautostart.h"
#include "xdgenv.h"

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
    QStringList config_dirs = XdgEnv::configDirs().split(":");
    for (int i = 0; i < config_dirs.count(); i++)
        pathList.append(config_dirs.at(i)+"/autostart/");
    pathList.push_back(XdgEnv::configHome()+"/autostart/");

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
        XdgDesktopFile * f = new XdgDesktopFile(_dir+filelist.at(i));
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
 * @brief constructor without parameters. XdgAutoStart will use its own Xdgenv!
 * Useful for using this class alone without a manager.
 */

XdgAutoStart::XdgAutoStart()
{
    qDebug() << "XdgAutoStart: initialising with on Xdgenv...";
    updateList();
}


/**
 * @brief returns a pointer to the list of Desktopfiles we stored
 */
QList< XdgDesktopFile* > XdgAutoStart::list()
{
    return m_list;
}
