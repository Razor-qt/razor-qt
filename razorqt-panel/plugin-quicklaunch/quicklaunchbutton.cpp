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
#include "razorquicklaunch.h"

#define MIMETYPE "x-razor/quicklaunch-button"


QuickLaunchButton::QuickLaunchButton(int id, QuickLaunchAction * act, QWidget * parent)
    : QToolButton(parent),
      m_act(act),
      m_id(id)
{
    setAcceptDrops(true);

    setDefaultAction(m_act);
    m_act->setParent(this);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setIconSize(QSize(22, 22));

    m_moveLeftAct = new QAction(XdgIcon::fromTheme("go-previous"), tr("Move left"), this);
    connect(m_moveLeftAct, SIGNAL(triggered()), this, SIGNAL(movedLeft()));

    m_moveRightAct = new QAction(XdgIcon::fromTheme("go-next"), tr("Move right"), this);
    connect(m_moveRightAct, SIGNAL(triggered()), this, SIGNAL(movedRight()));


    m_deleteAct = new QAction(XdgIcon::fromTheme("dialog-close"), tr("Remove from quicklaunch"), this);
    connect(m_deleteAct, SIGNAL(triggered()), this, SLOT(selfRemove()));
    addAction(m_deleteAct);
    m_menu = new QMenu(this);
    m_menu->addAction(m_act);
    m_menu->addSeparator();
    m_menu->addAction(m_moveLeftAct);
    m_menu->addAction(m_moveRightAct);
    m_menu->addSeparator();
    m_menu->addAction(m_deleteAct);


    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(this_customContextMenuRequested(const QPoint&)));
}

QuickLaunchButton::~QuickLaunchButton()
{
    //m_act->deleteLater();
}

QHash<QString,QString> QuickLaunchButton::settingsMap()
{
    Q_ASSERT(m_act);
    return m_act->settingsMap();
};


void QuickLaunchButton::this_customContextMenuRequested(const QPoint & pos)
{
    RazorQuickLaunch *panel = qobject_cast<RazorQuickLaunch*>(parent());

    m_moveLeftAct->setEnabled( panel && panel->indexOfButton(this) > 0);
    m_moveRightAct->setEnabled(panel && panel->indexOfButton(this) < panel->countOfButtons() - 1);
    m_menu->popup(mapToGlobal(pos));
}

void QuickLaunchButton::selfRemove()
{
    emit buttonDeleted(m_id);
}

void QuickLaunchButton::paintEvent(QPaintEvent *)
{
    // Do not paint that ugly "has menu" arrow
    QStylePainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    opt.features &= (~ QStyleOptionToolButton::HasMenu);
    p.drawComplexControl(QStyle::CC_ToolButton, opt);
}

void QuickLaunchButton::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton && e->modifiers() == Qt::ControlModifier)
    {
        m_dragStart = e->pos();
        return;
    }

    QToolButton::mousePressEvent(e);
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

    drag->exec(Qt::MoveAction);

    // Icon was droped outside the panel, remove button
    if (!drag->target())
    {
        selfRemove();
    }
}

void QuickLaunchButton::dragMoveEvent(QDragMoveEvent * e)
{
    if (e->mimeData()->hasFormat(MIMETYPE))
        e->acceptProposedAction();
    else
        e->ignore();
}

void QuickLaunchButton::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasFormat(MIMETYPE))
    {
        int other = e->mimeData()->data(MIMETYPE).toInt();
        // do not replace self with self
        if (other == m_id)
            return;

        emit switchButtons(m_id, other);
    }
}

