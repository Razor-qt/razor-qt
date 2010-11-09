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
Xdgmanager::Xdgmanager(QString _icontheme)
{
    qDebug() << "Xdgmanager: Initialising..." << _icontheme;
    //set and get variables first
    xdgenv = new Xdgenv();
    xdgenv->setEnv();

    /**
     * @todo here can maybe sometime follow a nicer implementation using the environmentals and stuff
     */
    QFile* testfile = new QFile(_icontheme);
    if (testfile->exists())
        xdgiconthememanager = new Xdgiconthememanager(_icontheme,xdgenv);
    else
    {
        delete testfile;
        testfile = new QFile("/usr/share/icons/nuvola/index.theme");
        if (testfile->exists())
            xdgiconthememanager = new Xdgiconthememanager("/usr/share/icons/nuvola/index.theme",xdgenv);
        else
        {
            delete testfile;
            testfile = new QFile("/usr/share/icons/gnome/index.theme");
            if (testfile->exists())
                xdgiconthememanager = new Xdgiconthememanager("/usr/share/icons/gnome/index.theme",xdgenv);
        }
    }
    xdgmenu = new Xdgmenu(xdgenv, xdgiconthememanager);
    xdgautostart = new Xdgautostart(xdgenv);
    delete testfile;

}

/**
 * @brief returns a pointer to the xdgautostart subsystem
 */
Xdgautostart* Xdgmanager::get_xdgautostart()
{
    return xdgautostart;
}





/**
 * @brief The destructor, deletes our stuff again
 */

Xdgmanager::~Xdgmanager()
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
Xdgenv* Xdgmanager::get_xdgenv()
{
    return xdgenv;
}

/**
 * @brief returns a pointer to the xdgmenu subsystem
 */

Xdgmenu* Xdgmanager::get_xdgmenu()
{
    return xdgmenu;
}


Xdgiconthememanager* Xdgmanager::get_xdgiconthememanager()
{
    return xdgiconthememanager;
}



#endif
