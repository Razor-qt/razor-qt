#ifndef XDGAUTOSTART_CPP
#define XDGAUTOSTART_CPP


//2010 Chris "VdoP" Regali

#include "xdgautostart.h"

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

void Xdgautostart::updateList()
{
  qDebug()<< "Xdgautostart: getting configpaths";
  QStringList pathList;
  QStringList config_dirs = autoEnv->getEnv("CONFIG_DIRS").split(":");
  for (int i = 0; i < config_dirs.count();i++)
    pathList.append(config_dirs.at(i)+"/autostart/");
  pathList.push_back(autoEnv->getEnv("CONFIG_HOME")+"/autostart/");
  
  qDebug() << "Xdgautostart: beginning building list";
  
  for (int i=0; i< pathList.length();i++)
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
void Xdgautostart::addDirtoList(QString _dir)
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
    Xdgdesktopfile myfile(_dir+filelist.at(i));
    myfile.parseFile();
    if (myfile.getValue("Hidden")=="true" || myfile.getValue("NotShowIn")=="razor" || 
	    ((myfile.getValue("OnlyShowIn") != "") && !myfile.getValue("OnlyShowIn").contains("razor")) )
      badNames.push_back(myfile.getValue("Name")); //this is needed as std says all with the same name get removed by one entry with these flags too
    else
      autoList.push_back(myfile);
  }
}

/**
 *@brief puts some output to qDebug()
 */

void Xdgautostart::debugAutostart()
{
    for (int i = 0; i < autoList.count(); i++)
    {
      Xdgdesktopfile tmp = autoList.at(i);
 	qDebug() <<tmp.getValue("Name");
    }
      
}






/**
 * @brief destructor
 * @todo Do This!
 */


Xdgautostart::~Xdgautostart()
{

}

/**
 * @brief does clean the list according to the XDG-Standards
 */

void Xdgautostart::cleanList()
{
  qDebug() << "Xdgautostart: cleaning house! (" << badNames.count() << " bad Names )";
  for (int i = 0; i < autoList.count(); i++)
  {
    Xdgdesktopfile tmp = autoList.at(i);
    if (badNames.contains(tmp.getValue("Name")))
      autoList.remove(i);
  }
  qDebug() << "Xdgautostart: after cleaning: " << autoList.count()<< " items left.";
}



/**
 * @brief constructor with parameters, Xdgmanager uses this
 * @param[in] _env The Xdgenv instance to use in this class
 */

Xdgautostart::Xdgautostart(Xdgenv* _env)
{
  qDebug() << "Xdgautostart: initialising with given Xdgenv...";
  autoEnv= _env;
}


/**
 * @brief constructor without parameters. Xdgautostart will use its own Xdgenv!
 * Useful for using this class alone without a manager.
 */

Xdgautostart::Xdgautostart()
{
  qDebug() << "Xdgautostart: initialising with on Xdgenv...";
  autoEnv = new Xdgenv;
}


/**
 * @brief returns a pointer to the list of Desktopfiles we stored
 */


QVector< Xdgdesktopfile >* Xdgautostart::get_List()
{
  return &autoList;
}





#endif
