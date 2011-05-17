/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
 * @brief declares class Razorworkspace and Razorworkspacemanager
 * @author Christopher "VdoP" Regali
 */

class WorkspaceConfig;
class ReadSettings;
class PowerManager;


/**
 * @brief This one is the actual workspace
 */

class RazorWorkSpace : public QGraphicsView
{
    Q_OBJECT
public:
    RazorWorkSpace(ReadSettings * config, int screen=-1, QWidget * parent=0);
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
    ReadSettings * m_config;
    int m_screen;
    Mode m_mode;
    
    RazorPluginInfoList mAvailablePlugins;

    QAction * m_actArrangeWidgets;
    QAction * m_actAddNewPlugin;
    QAction * m_actRemovePlugin;
    QAction * m_actConfigurePlugin;
    QAction * m_actSetbackground;
    QAction * m_actAbout;
    
    ArrangeItem * m_arrangeRoot;
    QList<ArrangeItem*> m_arrangeList;
    
    PowerManager * m_power;
    
    DesktopWidgetPlugin * getPluginFromItem(QGraphicsItem * item);
    QGraphicsItem * loadPlugin(QLibrary * lib, const QString & configId="default");
    void saveConfig();

private slots:
    void about();
    void workspaceResized(int screen);
    void arrangeWidgets(bool start);
    void showAddPluginDialog();
    void removePlugin();
    void configurePlugin();
    void setDesktopBackground();
    void addPlugin(RazorPluginInfo* pluginInfo);
};

#endif
