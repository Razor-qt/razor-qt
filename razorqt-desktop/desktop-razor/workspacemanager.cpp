#include <QtDebug>
#include <QApplication>
#include <QDesktopWidget>

#include "workspacemanager.h"

EXPORT_RAZOR_DESKTOP_PLUGIN_CPP(RazorWorkSpaceManager);


RazorWorkSpaceManager::RazorWorkSpaceManager(const QString & configId, ReadSettings * config)
    : DesktopPlugin(configId, config)
{
    qDebug() << "RazorWorkSpaceManager::RazorWorkSpaceManager";
    //this may actually make the icon work on multihead
    
    QSettings * m_settings = config->settings();
    m_settings->beginGroup(configId);
    
    QMap<int,WorkspaceConfig> desktops;
    WorkspaceConfig defaults(
                            strToBackgroundType(m_settings->value("wallpaper_type", "color").toString(), RazorWorkSpaceManager::BackgroundColor),
                            m_settings->value("wallpaper", "#006600").toString(),
                            m_settings->value("plugins", QStringList()).toStringList()
                        );
    int size = m_settings->beginReadArray("desktops");
    for (int i = 0; i < size; ++i) {
        m_settings->setArrayIndex(i);
        // TODO/FIXME: theme
        desktops[i] = WorkspaceConfig (
                            strToBackgroundType(m_settings->value("wallpaper_type").toString(), defaults.wallpaperType),
                            m_settings->value("wallpaper", defaults.wallpaper).toString(),
                            m_settings->value("plugins", defaults.plugins).toStringList()
                        );
    }
    m_settings->endArray();
    
    m_settings->endGroup();
    
    for (int i = 0; i < QApplication::desktop()->screenCount(); ++i)
    {
        qDebug() << "workspace for screen" << i;
        RazorWorkSpace * ws = new RazorWorkSpace(config, i);
        if (desktops.contains(i))
            ws->setConfig(desktops.value(i));
        else
            ws->setConfig(defaults);
        
        ws->show();
        m_workspaces.append(ws);
    }
}

RazorWorkSpaceManager::BackgroundType RazorWorkSpaceManager::strToBackgroundType(const QString& str, BackgroundType defaultValue) const
{
    if (str.toUpper() == "COLOR")  return BackgroundColor;
    if (str.toUpper() == "PIXMAP") return BackgroundPixmap;
    // failback
    if (str.toUpper() == "IMAGE") return BackgroundPixmap;
    return defaultValue;
}

RazorWorkSpaceManager::~RazorWorkSpaceManager()
{
    foreach (RazorWorkSpace* i, m_workspaces)
        delete i;
    m_workspaces.clear();
}

QString RazorWorkSpaceManager::info()
{
    return tr("Fully featured desktop implementation with all Razor's "
              "bells and whistles");
}
