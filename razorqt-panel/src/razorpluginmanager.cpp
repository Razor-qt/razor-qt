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
Razorpluginmanager::Razorpluginmanager()
{
    qDebug() << "Pluginmanager: initializing...";
    //get the location of the pluginconfig
    QString configfile = Razor::getInstance().get_settings()->getPath() +  Razor::getInstance().get_settings()->getValue("plugin_config");
    //read the plugin config into our Readsettings
    pluginsettings = new Readsettings(configfile);
    int num_panels = pluginsettings->getValue("num_bars").toInt();
    qDebug() << "Pluginmanager: Making " << num_panels << " panels!";
    //make the panels!
    QString I,W;
    for (int i=0; i < num_panels; i++)
    {
        Razor::getInstance().get_gui()->addPanel();
        //get the number of widgets for this panel
        I.setNum(i);
        int num_widgets = pluginsettings->getValue("num_widgets_"+I).toInt();
        qDebug() << "Pluginmanager: making " << num_widgets << " widgets of " << "num_widgets_"+I;
        for (int w = 0; w < num_widgets; w++)
        {
            W.setNum(w);
            QString plugin = pluginsettings->getValue("bar"+I+"_widget_"+W);
            qDebug() << "Pluginmanager: making plugin " << plugin << " out of " "bar"+I+"_widget_"+W;
            addPlugin(plugin,i);
        }
    }
}

/**
 * @brief adds a plugin to a bar
 */
void Razorpluginmanager::addPlugin(QString _plugin, int _bar)
{
    if (_plugin=="mainmenu")
    {
        Razormenu*  tmp = new Razormenu(_bar);
        pluginList.append(tmp);
    }
    else if (_plugin=="taskmanager")
    {
        Razortaskmanager* tmp = new Razortaskmanager(_bar);
        pluginList.append(tmp);
    }
    else if (_plugin =="desktopswitcher")
    {
        Razordeskswitch* tmp = new Razordeskswitch(_bar);
        pluginList.append(tmp);
    }
    else if (_plugin =="clock")
    {
        Razorclock* tmp = new Razorclock(_bar);
	pluginList.append(tmp);
    }
    else if (_plugin =="traybar")
    {
        Razortray* tmp = new Razortray(_bar);
	pluginList.append(tmp);
    }
    else if (_plugin =="razorcmd")
    {
	Razorcmd* tmp = new Razorcmd(_bar);
	pluginList.append(tmp);
    }
    else if (_plugin.contains("razorspinbutton"))
    {
        Razorspinbutton* tmp = new Razorspinbutton(_plugin,_bar);
	pluginList.append(tmp);
    }
    else if (_plugin =="logoutmenu")
    {
	Razorlogoutmenu* tmp = new Razorlogoutmenu(_bar);
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
Razorpluginmanager::~Razorpluginmanager()
{

}




#endif
