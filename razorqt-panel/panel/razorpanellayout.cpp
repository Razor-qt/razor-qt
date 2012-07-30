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


#include "razorpanellayout.h"
#include <QtCore/QSize>
#include <QtGui/QWidget>
#include <QtCore/QEvent>
#include <QtGui/QCursor>
#include <QtGui/QApplication>
#include <QDebug>
#include <QtCore/QPoint>
#include <QtGui/QMouseEvent>
#include <QtAlgorithms>
#include <QPoint>
#include <QMouseEvent>
#include <QPropertyAnimation>

#include "razorpanellimits.h"

// QVariantAnimation class was introduced in Qt 4.6.
#if QT_VERSION >= 0x040600
#define ANIMATION_ENABLE
#endif


#ifdef ANIMATION_ENABLE

#define ANIMATION_DURATION 250

class ItemMoveAnimation : public QVariantAnimation
{
public:
    ItemMoveAnimation(QLayoutItem *item) :
            mItem(item)
    {
        //setEasingCurve(QEasingCurve::InOutQuad); // <-- Classic animation style
        setEasingCurve(QEasingCurve::OutBack);   // <-- Modern animation style
        setDuration(ANIMATION_DURATION);
    }

    void updateCurrentValue(const QVariant &current)
    {
        mItem->setGeometry(current.toRect());
    }

private:
    QLayoutItem* mItem;
};
#endif


class MoveProcItem
{
public:
    MoveProcItem(QLayoutItem* item);
    ~MoveProcItem();

    QRect geometry() const { return mGeometry; }
    void move(QPoint topLeft);

private:
    QLayoutItem* mItem;
    QRect mGeometry;
#ifdef ANIMATION_ENABLE
    ItemMoveAnimation* mAnimation;
#endif
};


/************************************************

 ************************************************/
MoveProcessor::MoveProcessor(RazorPanelLayout* layout, QWidget* movedWidget):
    QWidget(movedWidget),
    mWidget(movedWidget),
    mLayout(layout)
{
    mWidgetPlace = mWidget->geometry();
    mOffset = QCursor::pos() - mWidget->pos();
    mIndex = layout->indexOf(movedWidget);
    mWidget->raise();

    for (int i=0; i<layout->count(); ++i)
    {
        if (i != mIndex)
            mItems.append(new MoveProcItem(layout->itemAt(i)));
    }

    QBoxLayout::Direction dir = layout->direction();
    mHoriz = (dir == QBoxLayout::LeftToRight || dir == QBoxLayout::RightToLeft);

    setMouseTracking(true);
    show(); //  Only visible widgets can grab mouse input.

    if (mHoriz) this->grabMouse(Qt::SizeHorCursor);
    else        this->grabMouse(Qt::SizeVerCursor);

    layout->setEnabled(false);
}


/************************************************

 ************************************************/
MoveProcessor::~MoveProcessor()
{
    qDeleteAll(mItems);
}


/************************************************

 ************************************************/
bool MoveProcessor::event(QEvent* event)
{
    switch (event->type())
    {
        case QEvent::MouseMove:
            if (mHoriz) mouseMoveHoriz();
            else        mouseMoveVert();

            event->accept();
            return true;

        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonDblClick:
            event->accept();
            return true;

        case QEvent::MouseButtonRelease:
            event->accept();
            apply();
            return true;


        case QEvent::ContextMenu:
            event->accept();
            return true;

        default:
            break;
    }

    return QWidget::event(event);
}


/************************************************

 ************************************************/
inline void MoveProcessor::mouseMoveHoriz()
{
    mWidget->move((QCursor::pos() - mOffset).x(), mWidget->pos().y());

    // ::::::::::::::::::::::
    QRect widgetGeometry = mWidget->geometry();

    // Check to left ..................
    for (int i = mIndex-1; i>-1; --i)
    {
        MoveProcItem* item = mItems[i];
        if (widgetGeometry.left() < item->geometry().center().x())
        {
            mIndex--;
            QPoint p = item->geometry().topLeft();
            p.rx() += widgetGeometry.width();
            item->move(p);
            mWidgetPlace.translate(- item->geometry().width(), 0);
        }
        else
            break;
    }


    // Check to right ..................
    for (int i = mIndex; i<mItems.size(); ++i)
    {
        MoveProcItem* item = mItems[i];
        if (widgetGeometry.right() > item->geometry().center().x())
        {
            mIndex++;
            QPoint p = item->geometry().topLeft();
            p.rx() -= widgetGeometry.width();
            item->move(p);
            mWidgetPlace.translate(item->geometry().width(), 0);
        }
        else
            break;
    }
}


/************************************************

 ************************************************/
inline void MoveProcessor::mouseMoveVert()
{
    mWidget->move(mWidget->pos().x(), (QCursor::pos() - mOffset).y());

    // ::::::::::::::::::::::
    QRect widgetGeometry = mWidget->geometry();

    // Check to top ..................
    for (int i = mIndex-1; i>-1; --i)
    {
        MoveProcItem* item = mItems[i];
        if (widgetGeometry.top() < item->geometry().center().y())
        {
            mIndex--;
            QPoint p = item->geometry().topLeft();
            p.ry() += widgetGeometry.height();
            item->move(p);
            mWidgetPlace.translate(0, -item->geometry().height());
        }
        else
            break;
    }


    // Check to bottom ................
    for (int i = mIndex; i<mItems.size(); ++i)
    {
        MoveProcItem* item = mItems[i];
        if ( widgetGeometry.bottom() > item->geometry().center().y())
        {
            mIndex++;
            QPoint p = item->geometry().topLeft();
            p.ry() -= widgetGeometry.height();
            item->move(p);
            mWidgetPlace.translate(0, item->geometry().height());
        }
        else
            break;
    }
}


/************************************************

 ************************************************/
void MoveProcessor::apply()
{
    int n = mLayout->indexOf(mWidget);

    QLayoutItem* item = mLayout->takeAt(n);
    mLayout->insertItem(mIndex, item);

#ifdef ANIMATION_ENABLE
    ItemMoveAnimation* animation = new ItemMoveAnimation(item);
    connect(animation, SIGNAL(finished()), this, SLOT(finished()));
    animation->setStartValue(item->geometry());
    animation->setEndValue(mWidgetPlace);
    animation->start(animation->DeleteWhenStopped);
#else
    finished();
#endif
}


/************************************************

 ************************************************/
void MoveProcessor::finished()
{
    releaseMouse();
    mLayout->setEnabled(true);
    // activate not work on Qt 4.7, but update not work on 4.6.
    mLayout->activate();
    mLayout->update();
    emit widgetMoved(mWidget);
    delete this;
}


#ifdef ANIMATION_ENABLE
/************************************************

 ************************************************/
MoveProcItem::MoveProcItem(QLayoutItem* item):
    mAnimation(0)
{
    mItem = item;
    mGeometry = item->geometry();
}


/************************************************

 ************************************************/
MoveProcItem::~MoveProcItem()
{
    delete mAnimation;
}


/************************************************

 ************************************************/
void MoveProcItem::move(QPoint topLeft)
{
    mGeometry.moveTopLeft(topLeft);
    if (!mAnimation)
        mAnimation = new ItemMoveAnimation(mItem);

    mAnimation->stop();
    mAnimation->setStartValue(mItem->geometry());
    mAnimation->setEndValue(mGeometry);
    mAnimation->start();
}


#else
/************************************************

 ************************************************/
MoveProcItem::MoveProcItem(QLayoutItem* item)
{
    mItem = item;
    mGeometry = item->geometry();
}


/************************************************

 ************************************************/
MoveProcItem::~MoveProcItem()
{
}

/************************************************

 ************************************************/
void MoveProcItem::move(QPoint topLeft)
{
    mGeometry.moveTopLeft(topLeft);
    mItem->setGeometry(mGeometry);
}
#endif


/************************************************

 ************************************************/
RazorPanelLayout::RazorPanelLayout(Direction dir, QWidget * parent):
    QBoxLayout(dir, parent)
{
    setContentsMargins(0, 0, 0, 0);
    setSpacing(0);
    setMargin(0);
}


/************************************************

 ************************************************/
RazorPanelLayout::~RazorPanelLayout()
{
}


/************************************************

 ************************************************/
void RazorPanelLayout::startMoveWidget(QWidget* widget)
{
    MoveProcessor* mp = new MoveProcessor(this, widget);
    connect(mp, SIGNAL(widgetMoved(QWidget*)), this, SIGNAL(widgetMoved(QWidget*)));
}


/************************************************

 ************************************************/
QSize RazorPanelLayout::sizeHint() const
{
    QSize size = QBoxLayout::sizeHint();
    if (isEmpty())
    {
        size.rheight() = qMax(size.rheight(), PANEL_MINIMUM_SIZE);
        size.rwidth() = qMax(size.rwidth(), PANEL_MINIMUM_SIZE);
    }
    return size;
}
