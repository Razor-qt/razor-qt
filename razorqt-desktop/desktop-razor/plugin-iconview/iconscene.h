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
