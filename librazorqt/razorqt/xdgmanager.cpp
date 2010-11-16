#ifndef XDGMAN_CPP
#define XDGMAN_CPP


#include "xdgmanager.h"

/**
 * @file xdgmanager.cpp
 * @brief Implements the class "Xdgmanager.h"
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  */


//2010 Chris "VdoP" Regali
//implements Xdgmanager



/**
 * @brief The constructor of the class,
 * initializes the components and sets our xdg-variables
 */
XdgManager::XdgManager(QString _icontheme)
{
    qDebug() << "Xdgmanager: Initialising..." << _icontheme;
    //set and get variables first
    xdgenv = new XdgEnv();
    xdgenv->setEnv();

    /**
     * @todo here can maybe sometime follow a nicer implementation using the environmentals and stuff
     */
    QFile* testfile = new QFile(_icontheme);
    if (testfile->exists())
        xdgiconthememanager = new XdgIconThemeManager(_icontheme,xdgenv);
    else
    {
        delete testfile;
        testfile = new QFile("/usr/share/icons/nuvola/index.theme");
        if (testfile->exists())
            xdgiconthememanager = new XdgIconThemeManager("/usr/share/icons/nuvola/index.theme",xdgenv);
        else
        {
            delete testfile;
            testfile = new QFile("/usr/share/icons/gnome/index.theme");
            if (testfile->exists())
                xdgiconthememanager = new XdgIconThemeManager("/usr/share/icons/gnome/index.theme",xdgenv);
        }
    }
    xdgmenu = new XdgMenu(xdgenv, xdgiconthememanager);
    xdgautostart = new XdgAutoStart(xdgenv);
    delete testfile;

}

/**
 * @brief returns a pointer to the xdgautostart subsystem
 */
XdgAutoStart* XdgManager::get_xdgautostart()
{
    return xdgautostart;
}





/**
 * @brief The destructor, deletes our stuff again
 */

XdgManager::~XdgManager()
{
    qDebug() << "Xdgmanager: dying";
    delete xdgiconthememanager;
    delete xdgmenu;
    delete xdgautostart;
    delete xdgenv;
}


/**
 * @brief returns a pointer to the xdgenv subsystem
 */
XdgEnv* XdgManager::get_xdgenv()
{
    return xdgenv;
}

/**
 * @brief returns a pointer to the xdgmenu subsystem
 */

XdgMenu* XdgManager::get_xdgmenu()
{
    return xdgmenu;
}


XdgIconThemeManager* XdgManager::get_xdgiconthememanager()
{
    return xdgiconthememanager;
}



#endif
