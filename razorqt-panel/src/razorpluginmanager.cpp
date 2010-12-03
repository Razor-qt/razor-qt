#ifndef RAZORPLUGINMANAGER_CPP
#define RAZORPLUGINMANAGER_CPP

#include "razorplugin.h"
#include "razorpluginmanager.h"
#include "razor.h"
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
    // remove numbers from the name to get raw library name.
    // Potential number goes into plugin in the "name" argument.
    QString strippedName(_plugin);
    strippedName.replace(QRegExp("[0-9]*"), "");
    // setup whole path to the library
    QString plugName(PLUGIN_DIR + QString("librazorpanel_") + strippedName + ".so");
    qDebug() << "PLUGIN: Loading plugin" << plugName;

    // check if the file exists. Probably debug only.
    QFileInfo fi(plugName);
    if (!fi.exists())
    {
        qDebug() << "PLUGIN: MISSING FILE";
        return;
    }

    QLibrary * dll = new QLibrary(plugName);
    PluginInitFunction initFunc = (PluginInitFunction) dll->resolve("init");
    if (!initFunc)
    {
        qDebug() << "PLUGIN: plugin: MISSING init()";
        delete dll;
        return;
    }

    plug = initFunc(panel, panel, _plugin);
    Q_ASSERT(plug);
    // now add the plug into the panel's layout.
    // it's easier to do it here instead to handle it in plugin itself
    panel->addWidget(plug, 0, Qt::AlignLeft);
    dllList.append(dll);
}

/**
 * @brief destructor
 */
RazorPluginManager::~RazorPluginManager()
{
    foreach(QLibrary *dll, dllList)
    {
        qDebug() << "DELETE dll" << dll->fileName();
        delete dll;
    }
}

#endif
