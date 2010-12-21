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
XdgManager::XdgManager(const QString & _icontheme)
{
    qDebug() << "Xdgmanager: Initialising..." << _icontheme;
    //set and get variables first
    xdgenv = new XdgEnv();
    xdgenv->setEnv();

    /**
     * @todo here can maybe sometime follow a nicer implementation using the environmentals and stuff
     */
    if (QFile::exists(_icontheme))
        xdgiconthememanager = new XdgIconThemeManager(_icontheme,xdgenv);
    else
    {
        QStringList failback;
        failback << "/usr/share/icons/oxygen/index.theme"
                 << "/usr/share/icons/Tango/index.theme"
                 << "/usr/share/icons/nuvola/index.theme"
                 << "/usr/share/icons/hicolor/index.theme";
        foreach (QString theme, failback)
        {
            qDebug() << "Looking for failback icon theme:" << theme;
            if (QFile::exists(theme))
            {
                xdgiconthememanager = new XdgIconThemeManager(theme, xdgenv);
                qDebug() << "    LOADED";
                break;
            }
            else
                qDebug() << "    skipped - not found";
        }
    }
    xdgmenu = new XdgMenu(xdgenv, xdgiconthememanager);
    xdgautostart = new XdgAutoStart(xdgenv);
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
