/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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


//#include <QStyleOptionToolBar>

#include "razorpanelplugin.h"
#include "razorpanelplugin_p.h"
#include "razorpanellayout.h"
#include <razorqt/razorplugininfo.h>
#include <razorqt/razorsettings.h>

#include <QStyleOptionToolBar>
#include <QtGui/QPainter>
#include <QtGui/QToolTip>
#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QMenu>
#include <qtxdg/xdgicon.h>
#include <QtCore/QMetaEnum>
#include <QtCore/QTranslator>


/************************************************

 ************************************************/
RazorPanelPlugin::RazorPanelPlugin(const RazorPanelPluginStartInfo* startInfo, QWidget* parent):
    QFrame(parent),
    d_ptr(new RazorPanelPluginPrivate(startInfo, this))
{

}


/************************************************

 ************************************************/
RazorPanelPluginPrivate::RazorPanelPluginPrivate(const RazorPanelPluginStartInfo* startInfo, RazorPanelPlugin* parent):
    QObject(parent),
    q_ptr(parent),
    mSettings(new RazorSettings(startInfo->settings, startInfo->configSection, this)),
    mConfigId(startInfo->configSection),
    mAlignmentCached(false),
    mMovable(false),
    mPanel(startInfo->panel)
{
    Q_Q(RazorPanelPlugin);

    q->setMinimumSize(1, 1);

    connect(mSettings, SIGNAL(settingsChanged()), q, SLOT(settingsChanged()));

    q->setWindowTitle(startInfo->pluginInfo.name());
    QBoxLayout* layout = new QBoxLayout(mPanel->isHorizontal() ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom, q);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    q->setLayout(layout);
    q->settingsChanged();
}


/************************************************

 ************************************************/
RazorPanelPlugin::~RazorPanelPlugin()
{
}


/************************************************

 ************************************************/
RazorPanelPluginPrivate::~RazorPanelPluginPrivate()
{
}


/************************************************

 ************************************************/
QSettings& RazorPanelPlugin::settings() const
{
    Q_D(const RazorPanelPlugin);
    return d->settings();
}


/************************************************

 ************************************************/
RazorPanel* RazorPanelPlugin::panel() const
{
    Q_D(const RazorPanelPlugin);
    return d->panel();
}


/************************************************

 ************************************************/
QMenu* RazorPanelPlugin::popupMenu() const
{

    QMenu* menu = new QMenu(windowTitle());
    //menu->setIcon(windowIcon());

    if (flags().testFlag(RazorPanelPlugin::HaveConfigDialog))
    {
        QAction* configAction = new QAction(tr("Configure"), menu);
        menu->addAction(configAction);
        connect(configAction, SIGNAL(triggered()), this, SLOT(showConfigureDialog()));
    }

    QAction* moveAction = new QAction(XdgIcon::fromTheme("transform-move"), tr("Move"), menu);
    menu->addAction(moveAction);
    connect(moveAction, SIGNAL(triggered()), this, SLOT(requestMove()));

    menu->addSeparator();

    QAction* removeAction = new QAction(XdgIcon::fromTheme("dialog-close"), tr("Remove"), menu);
    menu->addAction(removeAction);
    connect(removeAction, SIGNAL(triggered()), this, SLOT(requestRemove()));

    return menu;
}


/************************************************

 ************************************************/
void RazorPanelPlugin::requestMove()
{
    emit move();
}


/************************************************

 ************************************************/
void RazorPanelPlugin::requestRemove()
{
    emit remove();
}


/************************************************

 ************************************************/
void RazorPanelPlugin::addWidget(QWidget* widget)
{
    Q_D(RazorPanelPlugin);
    d->addWidget(widget);
}


/************************************************

 ************************************************/
void RazorPanelPluginPrivate::addWidget(QWidget* widget)
{
    Q_Q(RazorPanelPlugin);
    q->layout()->addWidget(widget);
}


/************************************************

 ************************************************/
void RazorPanelPlugin::layoutDirectionChanged(QBoxLayout::Direction direction)
{
    Q_D(RazorPanelPlugin);
    d->layoutDirectionChanged(direction);
}


/************************************************

 ************************************************/
void RazorPanelPluginPrivate::layoutDirectionChanged(QBoxLayout::Direction direction)
{
    Q_Q(RazorPanelPlugin);
    QBoxLayout* layout = qobject_cast<QBoxLayout*>(q->layout());
    if (layout)
        layout->setDirection(direction);
}


/************************************************

 ************************************************/
void RazorPanelPlugin::updateStyleSheet()
{
    Q_D(RazorPanelPlugin);
    d->updateStyleSheet();
}


/************************************************

 ************************************************/
void RazorPanelPluginPrivate::updateStyleSheet()
{
    Q_Q(RazorPanelPlugin);
    q->style()->unpolish(q);
    q->style()->polish(q);
}


/************************************************

 ************************************************/
void RazorPanelPluginPrivate::initStyleOption(QStyleOptionToolBar *option) const
{
    Q_Q(const RazorPanelPlugin);
    option->initFrom(q);
    option->state |= QStyle::State_Horizontal;
    if (mMovable)
        option->features = QStyleOptionToolBar::Movable;
}


/************************************************

 ************************************************/
QRect RazorPanelPluginPrivate::handleRect()
{
    Q_Q(RazorPanelPlugin);
    QStyleOptionToolBar opt;
    initStyleOption(&opt);

    return q->style()->subElementRect(QStyle::SE_ToolBarHandle, &opt, q);
}


/************************************************

 ************************************************/
bool RazorPanelPlugin::isMovable() const
{
    Q_D(const RazorPanelPlugin);
    return d->isMovable();
}


/************************************************

 ************************************************/
void RazorPanelPlugin::setMovable(bool movable)
{
    Q_D(RazorPanelPlugin);
    d->setMovable(movable);
    emit movableChanged(d->isMovable());
}


/************************************************

 ************************************************/
void RazorPanelPluginPrivate::setMovable(bool movable)
{
    if (mMovable == movable)
        return;

    Q_Q(RazorPanelPlugin);
    mMovable = movable;

    QMargins m = q->contentsMargins();

    if (movable)
        m.setLeft(m.left() + handleRect().width());
    else
        m.setLeft(m.left() - handleRect().width());

    q->setContentsMargins(m);

    updateStyleSheet();
}


/************************************************

 ************************************************/
void RazorPanelPlugin::x11EventFilter(XEvent* event)
{
    Q_UNUSED(event)
}


/************************************************

 ************************************************/
void RazorPanelPlugin::paintEvent(QPaintEvent* event)
{
    Q_D(RazorPanelPlugin);
    d->paintEvent(event);
}


/************************************************

 ************************************************/
void RazorPanelPluginPrivate::paintEvent(QPaintEvent* event)
{
    Q_Q(RazorPanelPlugin);

    if (mMovable)
    {
        QPainter p(q);
        QStyle *style = q->style();
        QStyleOptionToolBar opt;
        initStyleOption(&opt);

        opt.rect = style->subElementRect(QStyle::SE_ToolBarHandle, &opt, q);
        if (opt.rect.isValid())
            style->drawPrimitive(QStyle::PE_IndicatorToolBarHandle, &opt, &p, q);
    }
}


/************************************************

 ************************************************/
RazorPanelPlugin::Alignment RazorPanelPlugin::alignment() const
{
    Q_D(const RazorPanelPlugin);
    return d->alignment();
}


/************************************************

 ************************************************/
void RazorPanelPlugin::updateSizePolicy()
{
    if (panel()->isHorizontal())
        setSizePolicy(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
    else
        setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
}


/************************************************

 ************************************************/
RazorPanelPlugin::Alignment RazorPanelPluginPrivate::alignment() const
{
    if (!mAlignmentCached)
    {
        Q_Q(const RazorPanelPlugin);
        QString s = settings().value("alignment").toString();

        // Retrun default value
        if (s.isEmpty())
        {
            mAlignment = (q->flags().testFlag(RazorPanelPlugin::PreferRightAlignment)) ?
                        RazorPanelPlugin::AlignRight :
                        RazorPanelPlugin::AlignLeft;
        }
        else
            mAlignment = (s.toUpper() == "RIGHT") ?
                         RazorPanelPlugin::AlignRight :
                         RazorPanelPlugin::AlignLeft;

        mAlignmentCached = true;
    }

    return mAlignment;
}


/************************************************

 ************************************************/
void RazorPanelPlugin::setAlignment(Alignment alignment)
{
    Q_D(RazorPanelPlugin);
    d->setAlignment(alignment);
    emit alignmentChanged();
}


/************************************************

 ************************************************/
void RazorPanelPluginPrivate::setAlignment(RazorPanelPlugin::Alignment alignment)
{
    mAlignment = alignment;
    updateStyleSheet();
}


/************************************************

 ************************************************/
QString RazorPanelPlugin::configId() const
{
    Q_D(const RazorPanelPlugin);
    return d->configId();
}

bool RazorPanelPlugin::event(QEvent *e)
{
    Q_D(const RazorPanelPlugin);
    switch (e->type())
    {
        case QEvent::ContextMenu:
            d->mPanel->showPopupMenu(this);
            return true;
            break;

        default:
            break;
    }
    return QFrame::event(e);
}



