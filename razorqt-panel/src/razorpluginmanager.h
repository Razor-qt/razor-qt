#ifndef RAZORPLUGINMANAGER_H
#define RAZORPLUGINMANAGER_H

#include "defs.h"
#include <razorqt/readsettings.h>
#include "razorplugin.h"
/**
 * @file razorpluginmanager.cpp
 * @author Christopher "VdoP" Regali
 * @brief implements Razorpluginmanager
 */


/**
 * @brief manages the plugins
 */
class Razorpluginmanager
{
public:
    Razorpluginmanager();
    ~Razorpluginmanager();
    void addPlugin(QString _plugin, int _bar);
private:
    Readsettings* pluginsettings;
    QList<Razorplugin*> pluginList;
};




#endif
