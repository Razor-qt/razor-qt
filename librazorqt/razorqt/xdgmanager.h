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

class Xdgmanager : public QObject
{
    Q_OBJECT
public:
    Xdgmanager(QString _icontheme);
    ~Xdgmanager();
    Xdgenv* get_xdgenv();
    Xdgmenu* get_xdgmenu();
    Xdgautostart* get_xdgautostart();
    Xdgiconthememanager* get_xdgiconthememanager();

private:
    Xdgenv* xdgenv;
    Xdgmenu* xdgmenu;
    Xdgautostart* xdgautostart;
    Xdgiconthememanager* xdgiconthememanager;
};



#endif
