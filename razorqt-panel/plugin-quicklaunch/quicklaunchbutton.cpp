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


QuickLaunchButton::QuickLaunchButton(QuickLaunchAction * act, QWidget * parent)
    : QToolButton(parent),
      mAct(act)
{
    setAcceptDrops(true);

    setDefaultAction(mAct);
    mAct->setParent(this);
    setIconSize(QSize(22, 22));
    setMaximumSize(QUICKLAUNCH_BUTTON_SIZE, QUICKLAUNCH_BUTTON_SIZE);
    setMinimumSize(QUICKLAUNCH_BUTTON_SIZE, QUICKLAUNCH_BUTTON_SIZE);

    mMoveLeftAct = new QAction(XdgIcon::fromTheme("go-previous"), tr("Move left"), this);
    connect(mMoveLeftAct, SIGNAL(triggered()), this, SIGNAL(movedLeft()));

    mMoveRightAct = new QAction(XdgIcon::fromTheme("go-next"), tr("Move right"), this);
    connect(mMoveRightAct, SIGNAL(triggered()), this, SIGNAL(movedRight()));


    mDeleteAct = new QAction(XdgIcon::fromTheme("dialog-close"), tr("Remove from quicklaunch"), this);
    connect(mDeleteAct, SIGNAL(triggered()), this, SLOT(selfRemove()));
    addAction(mDeleteAct);
    mMenu = new QMenu(this);
    mMenu->addAction(mAct);
    mMenu->addSeparator();
    mMenu->addAction(mMoveLeftAct);
    mMenu->addAction(mMoveRightAct);
    mMenu->addSeparator();
    mMenu->addAction(mDeleteAct);


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
    Q_ASSERT(mAct);
    return mAct->settingsMap();
}


void QuickLaunchButton::this_customContextMenuRequested(const QPoint & pos)
{
    RazorQuickLaunch *panel = qobject_cast<RazorQuickLaunch*>(parent());

    mMoveLeftAct->setEnabled( panel && panel->indexOfButton(this) > 0);
    mMoveRightAct->setEnabled(panel && panel->indexOfButton(this) < panel->countOfButtons() - 1);
    mMenu->popup(mapToGlobal(pos));
}

void QuickLaunchButton::selfRemove()
{
    emit buttonDeleted();
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
        mDragStart = e->pos();
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

    if ((e->pos() - mDragStart).manhattanLength() < QApplication::startDragDistance())
    {
        return;
    }

    if (e->modifiers() != Qt::ControlModifier)
    {
        return;
    }

    QDrag *drag = new QDrag(this);
    ButtonMimeData *mimeData = new ButtonMimeData();
    mimeData->setButton(this);
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
    const ButtonMimeData *mimeData = qobject_cast<const ButtonMimeData*>(e->mimeData());
    if (mimeData && mimeData->button())
    {
        emit switchButtons(mimeData->button(), this);
    }
}

