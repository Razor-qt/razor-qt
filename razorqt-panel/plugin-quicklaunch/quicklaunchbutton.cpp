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

#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QStylePainter>
#include <QtGui/QStyleOptionToolButton>
#include <QtGui/QMouseEvent>
#include <QtGui/QApplication>

#include <qtxdg/xdgicon.h>

#include "quicklaunchbutton.h"

#define MIMETYPE "x-razor/quicklaunch-button"


QuickLaunchButton::QuickLaunchButton(int id, QuickLaunchAction * act, QWidget * parent)
    : QToolButton(parent),
      m_act(act),
      m_id(id)
{
    setAcceptDrops(true);

    setDefaultAction(m_act);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setIconSize(QSize(22, 22));

    m_deleteAct = new QAction(XdgIcon::fromTheme("dialog-close"), tr("Remove from quicklaunch"), this);
    connect(m_deleteAct, SIGNAL(triggered()), this, SLOT(selfRemove()));
    addAction(m_deleteAct);
    m_menu = new QMenu(this);
    m_menu->addAction(m_deleteAct);
    
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(this_customContextMenuRequested(const QPoint&)));
}

QuickLaunchButton::~QuickLaunchButton()
{
}

QHash<QString,QString> QuickLaunchButton::settingsMap()
{
    Q_ASSERT(m_act);
    return m_act->settingsMap();
};


void QuickLaunchButton::this_customContextMenuRequested(const QPoint & pos)
{
    m_menu->popup(mapToGlobal(pos));
}

void QuickLaunchButton::selfRemove()
{
    emit buttonDeleted(m_id);
}

void QuickLaunchButton::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    opt.features &= (~ QStyleOptionToolButton::HasMenu);
    p.drawComplexControl(QStyle::CC_ToolButton, opt);
}

void QuickLaunchButton::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        m_dragStart = e->pos();
    }
}

void QuickLaunchButton::mouseMoveEvent(QMouseEvent *e)
{
    if (!(e->buttons() & Qt::LeftButton))
    {
        return;
    }

    if ((e->pos() - m_dragStart).manhattanLength() < QApplication::startDragDistance())
    {
        return;
    }

    if (e->modifiers() != Qt::ControlModifier)
    {
        return;
    }

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData();
    QByteArray ba;
    ba.setNum(m_id);
    mimeData->setData(MIMETYPE, ba);
    drag->setMimeData(mimeData);
    // TODO/FIXME: WTF? Why it does not work?
    drag->setDragCursor(QCursor(Qt::SizeHorCursor).pixmap(), Qt::MoveAction);

    drag->exec(Qt::MoveAction);
}

void QuickLaunchButton::dragMoveEvent(QDragMoveEvent * e)
{
    if (e->mimeData()->hasFormat(MIMETYPE))
        e->acceptProposedAction();
}

void QuickLaunchButton::dragEnterEvent(QDragEnterEvent *e)
{
    int other = e->mimeData()->data(MIMETYPE).toInt();
    // do not replace self with self
    if (other == m_id)
        return;

    emit switchButtons(m_id, other);
}

