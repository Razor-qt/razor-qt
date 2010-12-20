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
class RazorPluginManager
{
public:
    RazorPluginManager();
    ~RazorPluginManager();
    void addPlugin(const QString & plugin, const QString & configId, RazorBar * panel);
private:
    ReadSettings* cfg;
    QList<QLibrary*> dllList;

    void createPanel(const QString & configId);
};




#endif
