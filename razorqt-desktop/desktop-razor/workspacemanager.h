#ifndef WORKSPACEMANAGER_H
#define WORKSPACEMANAGER_H

#include <QObject>

#include "desktopplugin.h"
#include "razorworkspace.h"

class WorkspaceConfig;

/**
 * @brief this class does all the managing, like looking in the settings for a wallpaper
 */

class RazorWorkSpaceManager : public QObject, public DesktopPlugin
{
    Q_OBJECT
public:

    enum BackgroundType {
        BackgroundPixmap = 0,
        BackgroundColor = 1
    };
    
    RazorWorkSpaceManager(const QString & configId, ReadSettings * config);
    ~RazorWorkSpaceManager();
    
    QString info();

private:
    QList<RazorWorkSpace*> m_workspaces;
    QSettings * m_settings;

    RazorWorkSpaceManager::BackgroundType strToBackgroundType(const QString& str, BackgroundType defaultValue) const;
};

struct WorkspaceConfig
{
    WorkspaceConfig() {};
    WorkspaceConfig(RazorWorkSpaceManager::BackgroundType _wallpaperType,
                    bool _keepAspectRatio,
                    const QString & _wallpaper,
                    const QStringList & _plugins)
    {
        wallpaperType = _wallpaperType;
        keepAspectRatio = _keepAspectRatio;
        wallpaper = _wallpaper;
        plugins = _plugins;
    }
    RazorWorkSpaceManager::BackgroundType wallpaperType;
    bool keepAspectRatio;
    QString wallpaper;
    QStringList plugins;
};


EXPORT_RAZOR_DESKTOP_PLUGIN_H

#endif
