#ifndef RAZORPLUGINMANAGER_CPP
#define RAZORPLUGINMANAGER_CPP
#include "razorplugin.h"
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
#include "razorbar.h"
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
        RazorBar * panel = Razor::getInstance().get_gui()->panel(i);
        int num_widgets = pluginsettings->getInt("num_widgets_"+I);
        qDebug() << "Pluginmanager: making " << num_widgets << " widgets of " << "num_widgets_"+I;
        for (int w = 0; w < num_widgets; w++)
        {
            W.setNum(w);
            QString plugin = pluginsettings->getString("bar"+I+"_widget_"+W);
            qDebug() << "Pluginmanager: making plugin " << plugin << " out of " "bar"+I+"_widget_"+W;
            addPlugin(plugin, panel);
        }
        // ensure all will be layouted correctly
        panel->pluginSizeChanged();
    }
}

/**
 * @brief adds a plugin to a bar
 */
void RazorPluginManager::addPlugin(QString _plugin, RazorBar * panel)
{
    RazorPlugin * plug;
    if (_plugin=="mainmenu")
    {
        plug = new RazorMenu(panel, panel);
        pluginList.append(plug);
    }
    else if (_plugin=="taskmanager")
    {
        plug = new RazorTaskManager(panel, panel);
        pluginList.append(plug);
    }
    else if (_plugin =="desktopswitcher")
    {
        plug = new RazorDeskSwitch(panel, panel);
        pluginList.append(plug);
    }
    else if (_plugin =="clock")
    {
        plug = new RazorClock(panel, panel);
        pluginList.append(plug);
    }
    else if (_plugin =="traybar")
    {
        plug = new RazorTray(panel, panel);
        pluginList.append(plug);
    }
    else if (_plugin =="razorcmd")
    {
        plug = new RazorCmd(panel, panel);
        pluginList.append(plug);
    }
    else if (_plugin.contains("razorspinbutton"))
    {
        plug = new RazorSpinButton(_plugin, panel, panel);
        pluginList.append(plug);
    }
    else if (_plugin =="logoutmenu")
    {
        plug = new RazorLogoutMenu(panel, panel);
        pluginList.append(plug);
    }
    else if (_plugin.contains("quicklaunch"))
    {
        plug = new RazorQuickLaunch(_plugin, panel, panel);
        pluginList.append(plug);
    }
    else if (_plugin.contains("devicemanager"))
    {
        plug = new RazorDevicePlugin(panel, panel);
        pluginList.append(plug);
    }
    // now add the plug into the panel's layout.
    // it's easier to do it here instead to handle it in plugin itself
    panel->addWidget(plug, 0, Qt::AlignLeft);
}






/**
 * @brief destructor
 */
RazorPluginManager::~RazorPluginManager()
{

}




#endif
