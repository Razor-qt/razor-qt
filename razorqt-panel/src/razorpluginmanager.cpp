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
    //read the plugin config into our Readsettings
    cfg = new ReadSettings("panel");
    QStringList panelNames = cfg->settings()->childGroups();
    if (panelNames.count() == 0)
        panelNames << "default";

    foreach (QString panel, panelNames)
    {
        qDebug() << "Pluginmanager: Making panels:" << panel;
        createPanel(panel);
    }
}

void RazorPluginManager::createPanel(const QString & configId)
{
    int panelID = Razor::getInstance().get_gui()->addPanel(configId);
    qDebug() << "ID" << panelID;
    // TODO/FIXME: indexing is illogicall
    RazorBar * panel = Razor::getInstance().get_gui()->panel(panelID-1);

    QSettings * s = cfg->settings();
    s->beginGroup(configId);
    int count = s->beginReadArray("plugins");
    QString dll;
    QString dllConfigId;

    for (int i = 0; i < count; ++i)
    {
        s->setArrayIndex(i);
        dll = s->value("name", "").toString();
        dllConfigId = s->value("config", "").toString();
        addPlugin(dll, dllConfigId, panel);
    }
    s->endArray();

    // simple defaults - to show something only if there is no config...
    if (count == 0)
    {
        qDebug() << "No plugins in config id:" << configId << "using defaults";
        addPlugin("desktopswitcher", "default", panel);
        addPlugin("taskmanager", "default", panel);
        addPlugin("clock", "default", panel);
    }
    // ensure all will be layouted correctly
    panel->pluginSizeChanged();
    s->endGroup();
}

/**
 * @brief adds a plugin to a bar
 */
void RazorPluginManager::addPlugin(const QString & plugin, const QString & configId, RazorBar * panel)
{
    RazorPlugin * plug;
    // setup whole path to the library
    QString plugName(PLUGIN_DIR + QString("librazorpanel_") + plugin + ".so");
    qDebug() << "PLUGIN: Loading plugin" << plugName << "config id:" << configId;

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

    plug = initFunc(panel, panel, configId);
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
