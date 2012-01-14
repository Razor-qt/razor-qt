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

#ifndef RAZORWORKSPACE_H
#define RAZORWORKSPACE_H

#include <QTextStream>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QAction>

#include <razorqt/xfitman.h>
#include <razorqt/razorplugininfo.h>
#include "arrangeitem.h"


/**
 * @file razorworkspace.h
 * @brief declares class Razorworkspace
 * @author Christopher "VdoP" Regali
 */

class WorkspaceConfig;
class RazorSettings;
class PowerManager;
class ScreenSaver;


/**
 * @brief This one is the actual workspace
 */

class RazorWorkSpace : public QGraphicsView
{
    Q_OBJECT
public:
    RazorWorkSpace(RazorSettings * config, int screen, int desktop, QWidget * parent=0);
    ~RazorWorkSpace();

    void setConfig(const WorkspaceConfig & bg);
    WorkspaceConfig getConfig();

protected:
    void mouseReleaseEvent(QMouseEvent* _ev);
    void wheelEvent(QWheelEvent* e);

private:
    
    enum Mode {
        ModeNormal,
        ModeArrange
    };

    QGraphicsScene * m_scene;
    RazorSettings * m_config;
    QString m_menuFile;
    bool m_wheelDesktopSwitch;
    int m_screen;
    int m_desktop;
    Mode m_mode;
    
    QAction * m_actArrangeWidgets;
    QAction * m_actAddNewPlugin;
    QAction * m_actRemovePlugin;
    QAction * m_actConfigurePlugin;
    QAction * m_actSetbackground;
    QAction * m_actAbout;
    
    ArrangeItem * m_arrangeRoot;
    QList<ArrangeItem*> m_arrangeList;
    
    PowerManager * m_power;
    ScreenSaver * m_screenSaver;
    
    DesktopWidgetPlugin * getPluginFromItem(QGraphicsItem * item);
    QGraphicsItem * loadPlugin(QLibrary * lib, const QString & configId="default");
    void saveConfig();
    QStringList pluginDesktopDirs();
    QLibrary* loadPluginLib(const RazorPluginInfo &pluginInfo);

private slots:
    void about();
    void workspaceResized(int screen);
    void arrangeWidgets(bool start);
    void showAddPluginDialog();
    void removePlugin();
    void configurePlugin();
    void setDesktopBackground();
    void addPlugin(const RazorPluginInfo &pluginInfo);
};

#endif
