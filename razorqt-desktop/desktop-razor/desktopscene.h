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

#ifndef DESKTOPSCENE_H
#define DESKTOPSCENE_H

#include <QtGui/QGraphicsScene>
#include <qtxdg/xdgmenu.h>

#include "desktopwidgetplugin.h"

class QLibrary;
class QAction;
class PowerManager;
class ScreenSaver;
class RazorPluginInfo;


class DesktopScene : public QGraphicsScene
{
    Q_OBJECT
    
public:
    DesktopScene(QObject * parent);

    static DesktopWidgetPlugin * getPluginFromItem(QGraphicsItem * item);

signals:
    void saveConfig();

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *e);
    void wheelEvent(QGraphicsSceneWheelEvent * e);


private:
    QString m_menuFile;
    bool m_wheelDesktopSwitch;
    QMenu *m_menu;
    XdgMenu m_xdgMenu;
    PowerManager * m_power;
    ScreenSaver * m_screenSaver;

    QAction * m_actArrangeWidgets;
    QAction * m_actAddNewPlugin;
    QAction * m_actRemovePlugin;
    QAction * m_actConfigurePlugin;
    QAction * m_actSetbackground;
    QAction * m_actAbout;

    DesktopWidgetPlugin *m_activePlugin;

    // map <plugin instance id - plugin>
    QMap<QString, DesktopWidgetPlugin*> m_plugins;
    // QLibrary cache; map <RazorPluginInfo id - library>
    QMap<QString, QLibrary *> m_libraries;

    DesktopWidgetPlugin * getPluginFromPoint(const QPointF &point);

    QStringList pluginDesktopDirs();

    DesktopWidgetPlugin * loadPlugin(QLibrary * lib, const QString & configId="default");
    QLibrary* loadPluginLib(const RazorPluginInfo &pluginInfo);

    void save();

private slots:
    void buildMenu();
    void showAddPluginDialog();
    void addPlugin(const RazorPluginInfo &pluginInfo);
    void removePlugin(bool ask=true);
    void configurePlugin();
    void about();
    void arrangeWidgets(bool enable);
    void setDesktopBackground();
};

#endif
