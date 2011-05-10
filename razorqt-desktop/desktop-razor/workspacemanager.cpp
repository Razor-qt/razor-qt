#include <QtDebug>
#include <QApplication>
#include <QDesktopWidget>

#include "workspacemanager.h"

EXPORT_RAZOR_DESKTOP_PLUGIN_CPP(RazorWorkSpaceManager);


RazorWorkSpaceManager::RazorWorkSpaceManager(const QString & configId, ReadSettings * config, ReadTheme * theme)
    : DesktopPlugin(configId, config, theme)
{
    qDebug() << "RazorWorkSpaceManager::RazorWorkSpaceManager" << configId;
    //this may actually make the icon work on multihead
    
    QSettings * m_settings = config->settings();

    m_settings->beginGroup(configId);
    
    QMap<int,WorkspaceConfig> desktops;
    WorkspaceConfig defaults(
                            strToBackgroundType(m_settings->value("wallpaper_type", "color").toString(), RazorWorkSpaceManager::BackgroundColor),
                            false,
                            m_settings->value("wallpaper", "#006600").toString(),
                            m_settings->value("plugins", QStringList()).toStringList()
                        );
    // important: here is used screenCount() instead of beginReadArray()
    // QSettings can contain more/les/or none desktop defined.
    m_settings->beginReadArray("desktops");
    int size = QApplication::desktop()->screenCount();
    QString themeWallpaper;
    for (int i = 0; i < size; ++i) {
        m_settings->setArrayIndex(i);
        themeWallpaper = theme->desktopBackground(i+1);
        desktops[i] = WorkspaceConfig (
                            strToBackgroundType(m_settings->value("wallpaper_type", themeWallpaper.isEmpty() ? "color" : "pixmap").toString(), RazorWorkSpaceManager::BackgroundColor),
                            m_settings->value("keep_aspect_ratio", defaults.keepAspectRatio).toBool(),
                            m_settings->value("wallpaper", themeWallpaper.isEmpty() ? defaults.wallpaper : themeWallpaper).toString(),
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
