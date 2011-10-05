/* BEGIN_COMMON_COPYRIGHT_HEADER
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
 * version 3 of the License, or (at your option) any later version.
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

#include <QtDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <razorqt/razorsettings.h>

#include "workspacemanager.h"

EXPORT_RAZOR_DESKTOP_PLUGIN_CPP(RazorWorkSpaceManager);


RazorWorkSpaceManager::RazorWorkSpaceManager(const QString & configId, RazorSettings * config)
    : DesktopPlugin(configId, config)
{
    qDebug() << "RazorWorkSpaceManager::RazorWorkSpaceManager" << configId;
    
    m_config->beginGroup(configId);
    
    QMap<int,WorkspaceConfig> desktops;
    WorkspaceConfig defaults(
                            strToBackgroundType(m_config->value("wallpaper_type", "color").toString(), RazorWorkSpaceManager::BackgroundColor),
                            false,
                            m_config->value("wallpaper", "#006600").toString(),
                            m_config->value("plugins", QStringList()).toStringList()
                        );
    // important: here is used screenCount() instead of beginReadArray()
    // QSettings can contain more/les/or none desktop defined.
    m_config->beginReadArray("desktops");
    int size = QApplication::desktop()->screenCount();
    QString themeWallpaper;
    for (int i = 0; i < size; ++i) {
        m_config->setArrayIndex(i);
        themeWallpaper = razorTheme->desktopBackground(i+1);

        desktops[i] = WorkspaceConfig (
                            strToBackgroundType(m_config->value("wallpaper_type", themeWallpaper.isEmpty() ? "color" : "pixmap").toString(), RazorWorkSpaceManager::BackgroundColor),
                            m_config->value("keep_aspect_ratio", defaults.keepAspectRatio).toBool(),
                            m_config->value("wallpaper", themeWallpaper.isEmpty() ? defaults.wallpaper : themeWallpaper).toString(),
                            m_config->value("plugins", defaults.plugins).toStringList()
                        );
    }
    m_config->endArray();
    
    m_config->endGroup();
    
    for (int i = 0; i < QApplication::desktop()->screenCount(); ++i)
    {
        qDebug() << "workspace for screen" << i;
        RazorWorkSpace * ws = new RazorWorkSpace(config, i);
        if (desktops.contains(i))
            ws->setConfig(desktops.value(i));
        else
            ws->setConfig(defaults);
        
        ws->show();
        
        // This X11 call seems mandatory for issue #3:
        // "There is only one desktop when on plasma-desktop"
        // for some WM it is not enough to use Qt::WA_X11NetWmWindowTypeDesktop
        // Note: this has to be called after is ws shown to take an effect.
        // EWMH specs: Cardinal to determine the desktop the window is in (or wants to be) starting with 0 for the first desktop. A Client MAY choose not to set this property, in which case the Window Manager SHOULD place it as it wishes. 0xFFFFFFFF indicates that the window SHOULD appear on all desktops.
        xfitMan().moveWindowToDesktop(ws->winId(), 0xFFFFFFFF);
        
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
