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


#ifndef RAZORQUICKLAUNCHBUTTON_H
#define RAZORQUICKLAUNCHBUTTON_H

#include <QtGui/QToolButton>

#include "quicklaunchaction.h"


class QuickLaunchButton : public QToolButton
{
    Q_OBJECT

public:
    QuickLaunchButton(int id, QuickLaunchAction * act, QWidget* parent = 0);
    ~QuickLaunchButton();

    QHash<QString,QString> settingsMap();
    int id() { return m_id; };

signals:
    void buttonDeleted(int index);
    void switchButtons(int first, int second);
    void movedLeft();
    void movedRight();

protected:
    //! Disable that annoying small arrow when there is a menu
    virtual void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent * e);

private:
    QuickLaunchAction *m_act;
    QAction *m_deleteAct;
    QAction *m_moveLeftAct;
    QAction *m_moveRightAct;
    QMenu *m_menu;
    QPoint m_dragStart;
    int m_id;

private slots:
    void this_customContextMenuRequested(const QPoint & pos);
    void selfRemove();
};

#endif
