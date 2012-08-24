/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef WORKSPACEMANAGER_H
#define WORKSPACEMANAGER_H

#include <QtCore/QObject>

#include "desktopplugin.h"
#include "razorworkspace.h"

class WorkspaceConfig;
class DesktopScene;


class DesktopConfig
{
public:
    static DesktopConfig* instance()
    {
        if (!m_instance)
            m_instance = new DesktopConfig;
        return m_instance;
    }

    RazorSettings *config;
    QString configId;
private:
    DesktopConfig(){};
    DesktopConfig(DesktopConfig const&){};
    //DesktopConfig& operator=(DesktopConfig const&){  };
    static DesktopConfig* m_instance;
};


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
    
    RazorWorkSpaceManager(const QString & configId, RazorSettings * config);
    ~RazorWorkSpaceManager();
    
    QString info();


protected:
    virtual void x11EventFilter(XEvent* event);

private:
    QList< QList<RazorWorkSpace*> > m_workspaces;
    DesktopScene *m_scene;
    int m_desktopCount;

    RazorWorkSpaceManager::BackgroundType strToBackgroundType(const QString& str, BackgroundType defaultValue) const;

private slots:
    void setup();
};

struct WorkspaceConfig
{
    WorkspaceConfig() {};
    WorkspaceConfig(RazorWorkSpaceManager::BackgroundType _wallpaperType,
                    bool _keepAspectRatio,
                    const QString & _wallpaper)
    {
        wallpaperType = _wallpaperType;
        keepAspectRatio = _keepAspectRatio;
        wallpaper = _wallpaper;
    }
    RazorWorkSpaceManager::BackgroundType wallpaperType;
    bool keepAspectRatio;
    QString wallpaper;
};


EXPORT_RAZOR_DESKTOP_PLUGIN_H

#endif
