#ifndef RAZORPLUGINMANAGER_CPP
#define RAZORPLUGINMANAGER_CPP
#include "razorpluginmanager.h"
#include "razor.h"
#include "razormainmenu.h"
#include "razortask.h"
#include "razordeskswitch.h"
#include "razorclock.h"
#include "razortray.h"
#include "razorcmd.h"
#include "razorspinbutton.h"
#include "razorlogoutmenu.h"
#include "razorquicklaunch.h"
#include "razordevplugin.h"
/**
 * @file razorpluginmanager.cpp
 * @author Christopher "VdoP" Regali
 * @brief implements Razorpluginmanager
 */


/**
 * @brief constructor - initializes everything with the plugins
 */
RazorPluginManager::RazorPluginManager()
{
    qDebug() << "Pluginmanager: initializing...";
    //get the location of the pluginconfig
    QString configfile = Razor::getInstance().get_settings()->getPath() +  Razor::getInstance().get_settings()->getString("plugin_config");
    //read the plugin config into our Readsettings
    pluginsettings = new ReadSettings(configfile);
    int num_panels = pluginsettings->getInt("num_bars");
    qDebug() << "Pluginmanager: Making " << num_panels << " panels!";
    //make the panels!
    QString I,W;
    for (int i=0; i < num_panels; i++)
    {
        Razor::getInstance().get_gui()->addPanel();
        //get the number of widgets for this panel
        I.setNum(i);
        int num_widgets = pluginsettings->getInt("num_widgets_"+I);
        qDebug() << "Pluginmanager: making " << num_widgets << " widgets of " << "num_widgets_"+I;
        for (int w = 0; w < num_widgets; w++)
        {
            W.setNum(w);
            QString plugin = pluginsettings->getString("bar"+I+"_widget_"+W);
            qDebug() << "Pluginmanager: making plugin " << plugin << " out of " "bar"+I+"_widget_"+W;
            addPlugin(plugin,i);
        }
    }
}

/**
 * @brief adds a plugin to a bar
 */
void RazorPluginManager::addPlugin(QString _plugin, int _bar)
{
    if (_plugin=="mainmenu")
    {
        RazorMenu*  tmp = new RazorMenu(_bar);
        pluginList.append(tmp);
    }
    else if (_plugin=="taskmanager")
    {
        RazorTaskManager* tmp = new RazorTaskManager(_bar);
        pluginList.append(tmp);
    }
    else if (_plugin =="desktopswitcher")
    {
        RazorDeskSwitch* tmp = new RazorDeskSwitch(_bar);
        pluginList.append(tmp);
    }
    else if (_plugin =="clock")
    {
        RazorClock* tmp = new RazorClock(_bar);
        pluginList.append(tmp);
    }
    else if (_plugin =="traybar")
    {
        RazorTray* tmp = new RazorTray(_bar);
        pluginList.append(tmp);
    }
    else if (_plugin =="razorcmd")
    {
        RazorCmd* tmp = new RazorCmd(_bar);
        pluginList.append(tmp);
    }
    else if (_plugin.contains("razorspinbutton"))
    {
        RazorSpinButton* tmp = new RazorSpinButton(_plugin,_bar);
        pluginList.append(tmp);
    }
    else if (_plugin =="logoutmenu")
    {
        RazorLogoutMenu* tmp = new RazorLogoutMenu(_bar);
        pluginList.append(tmp);
    }
    else if (_plugin.contains("quicklaunch"))
    {
        RazorQuickLaunch * tmp = new RazorQuickLaunch(_plugin,_bar);
        pluginList.append(tmp);
    }
    else if (_plugin.contains("devicemanager"))
    {
        RazorDevicePlugin * tmp = new RazorDevicePlugin(_bar);
        pluginList.append(tmp);
    }
}






/**
 * @brief destructor
 */
RazorPluginManager::~RazorPluginManager()
{

}




#endif
