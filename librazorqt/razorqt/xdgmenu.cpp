#ifndef XDGMENU_CPP
#define XDMENU_CPP


//2010 Chris "VdoP" Regali


/**
 * @file xdgmenu.cpp
 * @brief Implements the class "Xdgmenu"
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  */
#include "xdgmenu.h"

/**
 * @brief The constructor of the class making a new instance of Xdgenv, if u use the menu class without a manager
 */
XdgMenu::XdgMenu()
{
    qDebug() << "Xdgmenu: Initialising... making new Xdgenv";
    menuEnv = new XdgEnv;
    getDirlist();
}


/**
 * @brief The constructor of the class using a given Xdgenv*
 * This is used by Xdgmanager or can be used by any other class alike
 */

XdgMenu::XdgMenu(XdgEnv* _env, XdgIconThemeManager* _iconman)
{
    qDebug() << "Xdgmenu: Initialising... using given Xdgenv";
    menuEnv = _env;
    menuIcon = _iconman;
    getDirlist();
}

/**
 * @brief The destructor of the Class
 * @todo Do This!
 */


XdgMenu::~XdgMenu()
{
    qDebug() << "Xdgmenu: Initialising...";
}

/**
 * @brief Returns a QVector of Xdgdesktopfiles
 * use this if u need direct access to the displayed items
 */
QVector<XdgDesktopFile*>* XdgMenu::get_xdgfilelist()
{
    return &filelist;
}

/**
 * @brief Gets the std datadirs and returns a list of all .directory files
 */

void XdgMenu::getDirlist()
{
    QStringList pathList = menuEnv->getEnv("DATA_DIRS").split(":");
    for (int i = 0; i < pathList.count(); i++)
    {
        QDirIterator dir_iter(pathList.at(i)+"/desktop-directories/", QDirIterator::Subdirectories);
        while (dir_iter.hasNext())
        {
            dir_iter.next(); // move to child directory
            QFileInfo dir_file(dir_iter.fileInfo());
            if (dir_file.absoluteFilePath().endsWith(".directory"))
            {
                dirMap[dir_file.baseName()+".directory"] = dir_file.absoluteFilePath();
            }
        }
    }
}


/**
 * @brief Returns the menu entries as QList of QMenu!
 */

QMenu* XdgMenu::get_QMenus()
{
    QString lang = QLocale::system().name(); // (it_IT)
    QString country = lang.section('_', 0, 0); // (it_IT -> it)
    qMenu = new QMenu("RootMenu");
    //let the nodes build up the tree themselves
    mainMenu.fillQMenu(qMenu,qMenulist);

    //reset the root-node 2 items down, because of the node-overhead of our tree
    qMenu = qMenulist.at(1);

    //then link the list to our runCmd!
    qDebug() << "Xdgmenu: menubuilt.. listcount: " << qMenulist.count();
    /*for (int i = 0; i < qMenulist.count();i++)
        connect(qMenulist.at(i), SIGNAL(triggered(QAction *)), this, SLOT(runCmd(QAction *)));
    */
    for (int i = 1; i < qMenulist.count(); i++)
    {
        qDebug() << "Xdgmenu: trying to parse file: " << dirMap[qMenulist.at(i)->title()];
        XdgDesktopFile* menuParse = new XdgDesktopFile(dirMap[qMenulist.at(i)->title()]);
        if (!menuParse->isUseable())
            qDebug() << "Xdgmenu: couldnt find .directory file: " << dirMap[qMenulist.at(i)->title()];
        else
        {
            menuParse->parseFile();
            QString title;
            title = menuParse->getValue("Name["+country+"]");
            if (title == "")
                title=menuParse->getValue("Name");
            qMenulist.at(i)->setTitle(title);
            qMenulist.at(i)->setIcon((QIcon)menuIcon->getIconpath(menuParse->getValue("Icon"),"","Categories"));
            // add some tooltip here maybe? maybe even localized, no problem
        }
    }

    connect(qMenu, SIGNAL(triggered(QAction *)), this, SLOT(runCmd(QAction*)));

    return qMenu;
}

/**
 * @brief this slot just executes the given command from the submenu, all submenus should be connected to this!
 */

void XdgMenu::runCmd(QAction* _action)
{
    QString cmd = _action->data().toString();
    QProcess::startDetached(cmd);
}



/**
 * @brief Reads xdg-menufiles from the standart env-paths
 * It then instances a new Xdgreader with the path of the menufile to use
 * @return true if menufile was found and parsed successfully, false if no useable menufile was found
 */
bool XdgMenu::readMenufile()
{
    qDebug() << "Reading Menufile!";
    qDebug() << menuEnv->getEnv("CONFIG_DIRS");
    QStringList config_dirs = menuEnv->getEnv("CONFIG_DIRS").split(":");
    qDebug() << config_dirs;
    bool gotMenu = false;
    QFile menufile(config_dirs.at(0)+"/menus/razor-applications.menu");
    menuEnv->debugEnv();
    for (int i = 0; i < config_dirs.count(); i++)
    {
        //try the razor-menufile... does not exist now but in the future.. well try some well-used files
        if (!menufile.exists())
            menufile.setFileName(config_dirs.at(i)+"/menus/applications.menu");
        else
            gotMenu=true;
        if (!menufile.exists())
            menufile.setFileName(config_dirs.at(i)+"/menus/gnome-applications.menu");
        else
            gotMenu=true;
        if (!menufile.exists())
        {
            qDebug() << "Xdgmenu: no usable menufiles found - trying next path";
        }
        else
            gotMenu=true;
    }
    if (gotMenu == false)
    {
        qDebug() << "[!!] Xdgmenu: NO USEABLE MENUCONFIGS FOUND - THIS IS CRITICAL! disable this widget!!";
        return false;
    }
    menuReader = new XdgReader(menufile.fileName(),mainMenu, NULL);
    qDebug() << "Xdgmenu: found useable menu " << menufile.fileName() << " and parsed it";

    return true;
}

/**
 * @brief Gets the XDG_CONFIG_PATHS from an Xdgenv and uses them.
 * It splits them into a QStringList and then calls parseFiles for every Item
 */
void XdgMenu::getFilelist()
{
    qDebug()<< "Xdgmenu: getting configpaths";
    QStringList pathList = menuEnv->getEnv("DATA_DIRS").split(":");
    for (int i=0; i< pathList.length(); i++)
    {
        parseFiles(pathList.at(i));
    }
    cleanList();
}

/**
 * @brief cleaning the list of all name-evens of the not shown items (xdg-standart)
 */


void XdgMenu::cleanList()
{
    qDebug() << "Xdgmenu: cleaning house! (" << badNames.count() << " bad Names )";
    for (int i = 0; i < filelist.count(); i++)
    {
        XdgDesktopFile* tmp = filelist.at(i);
        if (badNames.contains(tmp->getValue("Name")))
            filelist.remove(i);
    }
    qDebug() << "Xdgmenu: after cleaning: " << filelist.count()<< " items left.";
}
/**
 * @brief assigns all of the menu-elements their respective icons!
 */
void XdgMenu::assignIcons()
{
    QString myicon,mypath,iconpath;
    qDebug() << "Xdgmenu: called assignIcons, doing for " << filelist.count() << " items";
    for (int i = 0; i < filelist.count(); i++)
    {
        myicon = filelist.at(i)->getValue("Icon");
        mypath = filelist.at(i)->getOldPath();
        iconpath = menuIcon->getIconpath(myicon,mypath,"Applications");
        filelist.at(i)->setValue("Icon",iconpath);
    }
}


/**
 * @brief Reads  .desktop files from the dir in the parameter
 * It then parses all the .desktop files and makes a list of Xdgdesktopfile
 * @param[in] _dir the directory that will be searched for .desktop files
 */

void XdgMenu::parseFiles(QString _dir)
{
    qDebug()<<"Xdgmenu: parsing stuff in" << _dir;
    QDirIterator desktop_file_iter(_dir + "/applications/", QDirIterator::Subdirectories);
    int i = 0;
    while (desktop_file_iter.hasNext())
    {
        i++;
        desktop_file_iter.next();
        if (desktop_file_iter.fileName() == "." || desktop_file_iter.fileName() == "..")
            continue;
        if (desktop_file_iter.filePath().endsWith(".desktop")) //only use .desktop files!
        {
            XdgDesktopFile* tmp = new XdgDesktopFile(desktop_file_iter.filePath());
            tmp->parseFile();

            //do some standart-exclusions here




            if (tmp->getValue("Hidden")=="true" || tmp->getValue("NotShowIn")=="razor" ||
                    ((tmp->getValue("OnlyShowIn") != "") && !tmp->getValue("OnlyShowIn").contains("razor")) )
                badNames.push_back(tmp->getValue("Name")); //this is needed as std says all with the same name get removed by one entry with these flags too
            else
                filelist.push_back(tmp);
        }
    }
    qDebug() << "found: " << i << " items in" << _dir;
}

/**
 * @brief feeds all of the found desktopfiles in the filelist to the menu-tree
 */

void XdgMenu::feedTree()
{
    qDebug() << "Xdgmenu: beginning feeding tree!";
    int i =0;
    for (i = 0; i < filelist.count(); i++)
    {
        XdgDesktopFile* testfile = filelist.at(i);
        mainMenu.fitsHere(testfile);
    }


    qDebug() << "Xdgmenu: all found items ("<<i<< ") fitted in tree!";

}



#endif
