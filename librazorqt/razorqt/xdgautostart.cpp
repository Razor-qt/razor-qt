#ifndef XDGAUTOSTART_CPP
#define XDGAUTOSTART_CPP


//2010 Chris "VdoP" Regali

#include "xdgautostart.h"
#include "xdgenv.h"

/**
 * @file xdgautostart.cpp
 * @brief Implements the class xdgautostart
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  */


/**
 * @brief updates the list of autostart items
 * gets a list of paths and calls addDirtoList for each path!
 */

void XdgAutoStart::updateList()
{
    qDebug()<< "Xdgautostart: getting configpaths";
    QStringList pathList;
    QStringList config_dirs = XdgEnv::configDirs().split(":");
    for (int i = 0; i < config_dirs.count(); i++)
        pathList.append(config_dirs.at(i)+"/autostart/");
    pathList.push_back(XdgEnv::configHome()+"/autostart/");

    qDebug() << "Xdgautostart: beginning building list";

    for (int i=0; i< pathList.length(); i++)
    {
        addDirtoList(pathList.at(i));
    }
    qDebug() << "Xdgautostart: found "<<autoList.count()<< " items for the autostart...";
    cleanList();
}

/**
 * @brief adds a given dir to the autostartlist
 * @param[in] _dir the directory to search for files
 **/
void XdgAutoStart::addDirtoList(QString _dir)
{
    qDebug() << "Xdgautostart: adding "<<_dir << " to list!";
    //this is probably small so no dir-iterator.. just list them and work the list!
    QDir directory(_dir);
    QStringList filters;
    filters<<"*.desktop";
    directory.setNameFilters(filters); //filter the .desktop files only
    QStringList filelist = directory.entryList();
    for (int i =0; i < filelist.count(); i++)
    {
        XdgDesktopFile myfile(_dir+filelist.at(i));
        if (myfile.isShow())
            autoList.push_back(myfile);
        else
            badNames.push_back(myfile.value("Name").toString()); //this is needed as std says all with the same name get removed by one entry with these flags too
    }
}

/**
 *@brief puts some output to qDebug()
 */

void XdgAutoStart::debugAutostart()
{
    for (int i = 0; i < autoList.count(); i++)
    {
        XdgDesktopFile tmp = autoList.at(i);
        qDebug() <<tmp.value("Name").toString();
    }

}






/**
 * @brief destructor
 * @todo Do This!
 */


XdgAutoStart::~XdgAutoStart()
{

}

/**
 * @brief does clean the list according to the XDG-Standards
 */

void XdgAutoStart::cleanList()
{
    qDebug() << "Xdgautostart: cleaning house! (" << badNames.count() << " bad Names )";
    for (int i = 0; i < autoList.count(); i++)
    {
        XdgDesktopFile tmp = autoList.at(i);
        if (badNames.contains(tmp.value("Name").toString()))
            autoList.remove(i);
    }
    qDebug() << "Xdgautostart: after cleaning: " << autoList.count()<< " items left.";
}



/**
 * @brief constructor without parameters. Xdgautostart will use its own Xdgenv!
 * Useful for using this class alone without a manager.
 */

XdgAutoStart::XdgAutoStart()
{
    qDebug() << "Xdgautostart: initialising with on Xdgenv...";
}


/**
 * @brief returns a pointer to the list of Desktopfiles we stored
 */


QVector< XdgDesktopFile >* XdgAutoStart::get_List()
{
    return &autoList;
}





#endif
