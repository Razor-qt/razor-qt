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

class RazorDeskManagerLegacy:public RazorDeskManager
{
    Q_OBJECT
public:
    RazorDeskManagerLegacy(RazorWorkSpace* _workspace);
    ~RazorDeskManagerLegacy();
    void updateIconList();
    QList< RazorDeskIcon* > iconList();
};



#endif
