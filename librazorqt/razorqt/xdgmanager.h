#ifndef XDGMAN_H
#define XDGMAN_H

/**
 * @file xdgmanager.h
 * @brief Declares the class "Xdgmanager.h"
 * @date 9.9.2010
 * @author Christopher "VdoP" Regali
  */


// 2010 by Chris "VdoP" Regali


#include "xdgautostart.h"
#include "xdgiconthememanager.h"


/**
 * @brief manages all the xdg-components and subsystems
 */

class XdgManager : public QObject
{
    Q_OBJECT
public:
    XdgManager(const QString & _icontheme);
    ~XdgManager();
    XdgAutoStart* get_xdgautostart();
    XdgIconThemeManager* get_xdgiconthememanager();

private:
    XdgAutoStart* xdgautostart;
    XdgIconThemeManager* xdgiconthememanager;
};



#endif
