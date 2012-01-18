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

#include <QtDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <razorqt/razorsettings.h>

#include "workspacemanager.h"

EXPORT_RAZOR_DESKTOP_PLUGIN_CPP(RazorWorkSpaceManager);


RazorWorkSpaceManager::RazorWorkSpaceManager(const QString & configId, RazorSettings * config)
    : DesktopPlugin(configId, config)
    , m_configId(configId)
    , m_desktopCount(1)
{
    qDebug() << "RazorWorkSpaceManager::RazorWorkSpaceManager" << configId;
    setup();
}
    
void RazorWorkSpaceManager::setup()
{
    m_config->sync();
    m_config->beginGroup(m_configId);
    
    bool useDifferentWallpapers = m_config->value("use_different_wallpapers", false).toBool();

    QMap< int,QMap<int,WorkspaceConfig> > desktops;

    WorkspaceConfig defaults(
                            strToBackgroundType(m_config->value("wallpaper_type", "color").toString(), RazorWorkSpaceManager::BackgroundColor),
                            false,
                            m_config->value("wallpaper", "#006600").toString(),
                            m_config->value("plugins", QStringList()).toStringList()
                        );

    // important: here is used screenCount() instead of beginReadArray()
    // QSettings can contain more/les/or none desktop defined.
    m_config->beginReadArray("screens");
    int numScreens = QApplication::desktop()->screenCount();

    // Loop over screens
    for (int screen = 0; screen < numScreens; ++screen) {
       m_config->setArrayIndex(screen);
       m_config->beginReadArray("desktops");

       // Loop over virtual desktops
       for (int desktop = 0; desktop < m_desktopCount; ++desktop) {
           if (!useDifferentWallpapers && desktop > 0)
               break;

           m_config->setArrayIndex(desktop);
           QString themeWallpaper = razorTheme->desktopBackground(desktop + 1);

           desktops[screen][desktop] = WorkspaceConfig (
                               strToBackgroundType(m_config->value("wallpaper_type", themeWallpaper.isEmpty() ? "color" : "pixmap").toString(), RazorWorkSpaceManager::BackgroundColor),
                               m_config->value("keep_aspect_ratio", defaults.keepAspectRatio).toBool(),
                               m_config->value("wallpaper", themeWallpaper.isEmpty() ? defaults.wallpaper : themeWallpaper).toString(),
                               m_config->value("plugins", defaults.plugins).toStringList()
                           );
        }

        m_config->endArray();  // desktops
    }
    m_config->endArray();  // screens
     
    m_config->endGroup();
     
    // Remove workspaces of additional screens
    int toRemove = m_workspaces.count() - numScreens;
    for (int i = 0; i < toRemove; ++i) {
        qDeleteAll(m_workspaces.last());
        m_workspaces.removeLast();
    }

    // Loop over screens
    for (int screen = 0; screen < numScreens; ++screen) {
        QList<RazorWorkSpace*>* screenWorkspaces;
        if (screen < m_workspaces.count()) {
            // Existing screen
            screenWorkspaces = &m_workspaces[screen];
        }
        else {
            // New screen
            Q_ASSERT(screen == m_workspaces.count());
            m_workspaces.append(QList<RazorWorkSpace*>());
            screenWorkspaces = &m_workspaces.last();
        }

        // Remove additional workspaces
        int toRemove = screenWorkspaces->count() - m_desktopCount;
        for (int i = 0; i < toRemove; ++i)
            delete screenWorkspaces->takeLast();

        // Loop over virtual desktops
        for (int desktop = 0; desktop < m_desktopCount; ++desktop) {
            if (!useDifferentWallpapers && desktop > 0)
                break;

            qDebug() << "workspace screen: " << screen;
            qDebug() << "virtual desktop: " << desktop;

            // Use existing RazorWorkSpace instance or create a new one
            RazorWorkSpace * ws;
            if (desktop < screenWorkspaces->count()) {
                // Existing workspace
                ws = screenWorkspaces->at(desktop);
            }
            else {
                // New workspace
                Q_ASSERT(desktop == screenWorkspaces->count());
                ws = new RazorWorkSpace(m_config, screen, desktop);
                screenWorkspaces->append(ws);
            }

            if (desktops.contains(screen) && desktops[screen].contains(desktop))
                ws->setConfig(desktops.value(screen).value(desktop));
            else
                ws->setConfig(defaults);
            
            ws->show();
            
            // This X11 call seems mandatory for issue #3:
            // "There is only one desktop when on plasma-desktop"
            // for some WM it is not enough to use Qt::WA_X11NetWmWindowTypeDesktop
            // Note: this has to be called after is ws shown to take an effect.
            // EWMH specs: Cardinal to determine the desktop the window is in (or wants to be) starting with 0 for the first desktop. A Client MAY choose not to set this property, in which case the Window Manager SHOULD place it as it wishes. 0xFFFFFFFF indicates that the window SHOULD appear on all desktops.
            if (useDifferentWallpapers)
                xfitMan().moveWindowToDesktop(ws->winId(), desktop);
            else
                xfitMan().moveWindowToDesktop(ws->winId(), 0xFFFFFFFF);
        }
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
    foreach (const QList<RazorWorkSpace*>& screenWorkspaces, m_workspaces)
        qDeleteAll(screenWorkspaces);
}

QString RazorWorkSpaceManager::info()
{
    return tr("Fully featured desktop implementation with all Razor's "
              "bells and whistles");
}

void RazorWorkSpaceManager::x11EventFilter(XEvent* _event)
{
    if (_event->type == PropertyNotify)
    {
        int count = qMax(xfitMan().getNumDesktop(), 1);
        if (m_desktopCount != count)
        {
            qDebug() << "Desktop count changed from" << m_desktopCount << "to" << count;
            m_desktopCount = count;
            setup();
        }
    }
}
