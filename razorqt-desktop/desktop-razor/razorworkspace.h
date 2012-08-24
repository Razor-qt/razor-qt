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

#include <QtGui/QGraphicsView>

class WorkspaceConfig;
class BackgroundProvider;
class DesktopScene;


/**
 * @brief This one is the actual workspace
 */

class RazorWorkSpace : public QGraphicsView
{
    Q_OBJECT
public:
    RazorWorkSpace(DesktopScene *scene, int screen, int desktop, QWidget * parent=0);
    ~RazorWorkSpace();

    void setConfig(const WorkspaceConfig & bg);
    WorkspaceConfig getConfig();

    void setDesktopBackground();

public slots:
	void saveConfig();

protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent * event);
	void dropEvent(QDropEvent *event);

private:

    DesktopScene * m_scene;

    int m_screen;
    int m_desktop;
    
    BackgroundProvider *m_background;
    QGraphicsPixmapItem *m_backgroundItem;

private slots:
    void workspaceResized(int screen);
};

#endif
