#ifndef RAZORDESKMANL_H
#define RAZORDESKMANL_H

#include "defs.h"
#include "razordeskicon.h"
#include <razorqt/readsettings.h>
#include <razorqt/xdgdesktopfile.h>
#include <razorqt/xdgenv.h>
#include <razorqt/xdgiconthemefile.h>
#include <razorqt/xdgiconthememanager.h>

#include "razordeskman.h"


/**
 * @file razordeskmanl.h
 * @author Christopher "VdoP" Regali
 * @brief declares the class Razordeskmanagerlegacy
 */


/**
 * @brief this desktop-manager is pretty well known - it displays every .desktop file in ~/Desktop
 */

class Razordeskmanagerlegacy:public Razordeskmanager
{
  Q_OBJECT
public:
    Razordeskmanagerlegacy(Razorworkspace* _workspace);
    ~Razordeskmanagerlegacy();
    virtual void updateIconList();
    virtual QList< Razordeskicondata* >* iconList();
    virtual void showIcons();

private:
    void getFileList();
    QList<Xdgdesktopfile*> itemList;
};



#endif
