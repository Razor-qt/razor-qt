/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
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

#ifndef ICONSCENE_H
#define ICONSCENE_H

#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsSceneDragDropEvent>
#include "desktopplugin.h"

class QFileSystemWatcher;
class IconViewLabel;


class IconScene : public QGraphicsScene
{
    Q_OBJECT

public:
    IconScene(const QString & directory, QObject * parent = 0);

    void setParentSize(const QSizeF & size);

    bool blockGlobalMenu();
    QString dir() { return m_directory; }
    void setDir(const QString & directory);

private:
    QString m_directory;
    QFileSystemWatcher * m_fsw;
    QSizeF m_parentSize;
    DesktopPlugin::IconLaunchMode m_launchMode;

    void setDirImpl(const QString & directory, bool repaint=false);

    void dragEnterEvent(QGraphicsSceneDragDropEvent *e);
    //! It's required to have dragMoveEvent as QGraphicsScene needs it to handle drops.
    void dragMoveEvent(QGraphicsSceneDragDropEvent *e);
    void dropEvent(QGraphicsSceneDragDropEvent *e);

private slots:
    void updateIconList();
};

#endif
