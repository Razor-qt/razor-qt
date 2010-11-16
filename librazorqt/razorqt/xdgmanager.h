#ifndef XDGMAN_H
#define XDGMAN_H

/**
 * @file xdgmanager.h
 * @brief Declares the class "Xdgmanager.h"
 * @date 9.9.2010
 * @author Christopher "VdoP" Regali
  */


// 2010 by Chris "VdoP" Regali


#include "defs.h"

#include "xdgreader.h"
#include "xdgenv.h"
#include "xdgmenu.h"
#include "xdgautostart.h"
#include "xdgiconthememanager.h"


/**
 * @brief manages all the xdg-components and subsystems
 */

class XdgManager : public QObject
{
    Q_OBJECT
public:
    XdgManager(QString _icontheme);
    ~XdgManager();
    XdgEnv* get_xdgenv();
    XdgMenu* get_xdgmenu();
    XdgAutoStart* get_xdgautostart();
    XdgIconThemeManager* get_xdgiconthememanager();

private:
    XdgEnv* xdgenv;
    XdgMenu* xdgmenu;
    XdgAutoStart* xdgautostart;
    XdgIconThemeManager* xdgiconthememanager;
};



#endif
