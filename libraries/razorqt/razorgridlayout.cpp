/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
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


#include "razorgridlayout.h"
#include <QtGui/QWidget>
#include <QtCore/QDebug>
#include <math.h>

class RazorGridLayoutPrivate
{
public:
    RazorGridLayoutPrivate();

    QList<QLayoutItem*> mItems;
    int mRowCount;
    int mColumnCount;
    RazorGridLayout::Direction mDirection;

    bool mIsValid;
    QSize mCellSizeHint;
    QSize mCellMaxSize;
    int mVisibleCount;
    RazorGridLayout::Stretch mStretch;


    void updateCache();
    int rows() const;
    int cols() const;
    QSize mPrefCellMinSize;
    QSize mPrefCellMaxSize;
};


/************************************************

 ************************************************/
RazorGridLayoutPrivate::RazorGridLayoutPrivate()
{
    mColumnCount = 0;
    mRowCount = 0;
    mDirection = RazorGridLayout::LeftToRight;
    mIsValid = false;
    mVisibleCount = 0;
    mStretch = RazorGridLayout::StretchHoriz | RazorGridLayout::StretchVert;
    mPrefCellMinSize = QSize(0,0);
    mPrefCellMaxSize = QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}


/************************************************

 ************************************************/
void RazorGridLayoutPrivate::updateCache()
{
    mCellSizeHint = QSize(0, 0);
    mCellMaxSize = QSize(0, 0);
    mVisibleCount = 0;

    for (int i=0; i<mItems.count(); ++i)
    {
        QLayoutItem *item = mItems.at(i);
        if (!item->widget() || item->widget()->isHidden())
            continue;

        int h = qBound(item->minimumSize().height(),
                       item->sizeHint().height(),
                       item->maximumSize().height());

        int w = qBound(item->minimumSize().width(),
                       item->sizeHint().width(),
                       item->maximumSize().width());

        mCellSizeHint.rheight() = qMax(mCellSizeHint.height(), h);
        mCellSizeHint.rwidth()  = qMax(mCellSizeHint.width(), w);

        mCellMaxSize.rheight() = qMax(mCellMaxSize.height(), item->maximumSize().height());
        mCellMaxSize.rwidth()  = qMax(mCellMaxSize.width(), item->maximumSize().width());
        mVisibleCount++;

#if 0
        qDebug() << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-";
        qDebug() << "item.min" << item->minimumSize().width();
        qDebug() << "item.sz " << item->sizeHint().width();
        qDebug() << "item.max" << item->maximumSize().width();
        qDebug() << "w h" << w << h;
        qDebug() << "wid.sizeHint" << item->widget()->sizeHint();
        qDebug() << "mCellSizeHint:" << mCellSizeHint;
        qDebug() << "mCellMaxSize: " << mCellMaxSize;
        qDebug() << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-";
#endif

    }
    mCellSizeHint.rwidth() = qBound(mPrefCellMinSize.width(),  mCellSizeHint.width(),  mPrefCellMaxSize.width());
    mCellSizeHint.rheight()= qBound(mPrefCellMinSize.height(), mCellSizeHint.height(), mPrefCellMaxSize.height());
    mIsValid = !mCellSizeHint.isEmpty();
}


/************************************************

 ************************************************/
int RazorGridLayoutPrivate::rows() const
{
    if (mRowCount)
        return mRowCount;

    if (!mColumnCount)
        return 1;

    return ceil(mVisibleCount * 1.0 / mColumnCount);
}


/************************************************

 ************************************************/
int RazorGridLayoutPrivate::cols() const
{
    if (mColumnCount)
        return mColumnCount;

    int rows = mRowCount;
    if (!rows)
        rows = 1;

    return ceil(mVisibleCount * 1.0 / rows);
}



/************************************************

 ************************************************/
RazorGridLayout::RazorGridLayout(QWidget *parent):
    QLayout(parent),
    d_ptr(new RazorGridLayoutPrivate())
{
}


/************************************************

 ************************************************/
RazorGridLayout::~RazorGridLayout()
{
    delete d_ptr;
}


/************************************************

 ************************************************/
void RazorGridLayout::addItem(QLayoutItem *item)
{
    d_ptr->mItems.append(item);
}


/************************************************

 ************************************************/
QLayoutItem *RazorGridLayout::itemAt(int index) const
{
    Q_D(const RazorGridLayout);
    if (index < 0 || index >= d->mItems.count())
        return 0;

    return d->mItems.at(index);
}


/************************************************

 ************************************************/
QLayoutItem *RazorGridLayout::takeAt(int index)
{
    Q_D(RazorGridLayout);
    if (index < 0 || index >= d->mItems.count())
        return 0;

    QLayoutItem *item = d->mItems.takeAt(index);
    return item;
}


/************************************************

 ************************************************/
int RazorGridLayout::count() const
{
    Q_D(const RazorGridLayout);
    return d->mItems.count();
}


/************************************************

 ************************************************/
void RazorGridLayout::invalidate()
{
    Q_D(RazorGridLayout);
    d->mIsValid = false;
    QLayout::invalidate();
}


/************************************************

 ************************************************/
int RazorGridLayout::rowCount() const
{
    Q_D(const RazorGridLayout);
    return d->mRowCount;
}


/************************************************

 ************************************************/
void RazorGridLayout::setRowCount(int value)
{
    Q_D(RazorGridLayout);
    if (d->mRowCount != value)
    {
        d->mRowCount = value;
        invalidate();
    }
}


/************************************************

 ************************************************/
int RazorGridLayout::columnCount() const
{
    Q_D(const RazorGridLayout);
    return d->mColumnCount;
}


/************************************************

 ************************************************/
void RazorGridLayout::setColumnCount(int value)
{
    Q_D(RazorGridLayout);
    if (d->mColumnCount != value)
    {
        d->mColumnCount = value;
        invalidate();
    }
}


/************************************************

 ************************************************/
RazorGridLayout::Direction RazorGridLayout::direction() const
{
    Q_D(const RazorGridLayout);
    return d->mDirection;
}


/************************************************

 ************************************************/
void RazorGridLayout::setDirection(RazorGridLayout::Direction value)
{
    Q_D(RazorGridLayout);
    if (d->mDirection != value)
    {
        d->mDirection = value;
        invalidate();
    }
}

/************************************************

 ************************************************/
RazorGridLayout::Stretch RazorGridLayout::stretch() const
{
    Q_D(const RazorGridLayout);
    return d->mStretch;
}

/************************************************

 ************************************************/
void RazorGridLayout::setStretch(Stretch value)
{
    Q_D(RazorGridLayout);
    if (d->mStretch != value)
    {
        d->mStretch = value;
        invalidate();
    }
}


/************************************************

 ************************************************/
void RazorGridLayout::moveItem(int from, int to)
{
    Q_D(RazorGridLayout);
    d->mItems.move(from, to);
    invalidate();
}


/************************************************

 ************************************************/
QSize RazorGridLayout::cellMinimumSize() const
{
    Q_D(const RazorGridLayout);
    return d->mPrefCellMinSize;
}


/************************************************

 ************************************************/
void RazorGridLayout::setCellMinimumSize(QSize minSize)
{
    Q_D(RazorGridLayout);
    if (d->mPrefCellMinSize != minSize)
    {
        d->mPrefCellMinSize = minSize;
        invalidate();
    }
}


/************************************************

 ************************************************/
void RazorGridLayout::setCellMinimumHeight(int value)
{
    Q_D(RazorGridLayout);
    if (d->mPrefCellMinSize.height() != value)
    {
        d->mPrefCellMinSize.setHeight(value);
        invalidate();
    }
}


/************************************************

 ************************************************/
void RazorGridLayout::setCellMinimumWidth(int value)
{
    Q_D(RazorGridLayout);
    if (d->mPrefCellMinSize.width() != value)
    {
        d->mPrefCellMinSize.setWidth(value);
        invalidate();
    }
}


/************************************************

 ************************************************/
QSize RazorGridLayout::cellMaximumSize() const
{
    Q_D(const RazorGridLayout);
    return d->mPrefCellMaxSize;
}


/************************************************

 ************************************************/
void RazorGridLayout::setCellMaximumSize(QSize maxSize)
{
    Q_D(RazorGridLayout);
    if (d->mPrefCellMaxSize != maxSize)
    {
        d->mPrefCellMaxSize = maxSize;
        invalidate();
    }
}


/************************************************

 ************************************************/
void RazorGridLayout::setCellMaximumHeight(int value)
{
    Q_D(RazorGridLayout);
    if (d->mPrefCellMaxSize.height() != value)
    {
        d->mPrefCellMaxSize.setHeight(value);
        invalidate();
    }
}


/************************************************

 ************************************************/
void RazorGridLayout::setCellMaximumWidth(int value)
{
    Q_D(RazorGridLayout);
    if (d->mPrefCellMaxSize.width() != value)
    {
        d->mPrefCellMaxSize.setWidth(value);
        invalidate();
    }
}


/************************************************

 ************************************************/
void RazorGridLayout::setCellFixedSize(QSize size)
{
    Q_D(RazorGridLayout);
    if (d->mPrefCellMinSize != size ||
        d->mPrefCellMaxSize != size)
    {
        d->mPrefCellMinSize = size;
        d->mPrefCellMaxSize = size;
        invalidate();
    }
}


/************************************************

 ************************************************/
void RazorGridLayout::setCellFixedHeight(int value)
{
    Q_D(RazorGridLayout);
    if (d->mPrefCellMinSize.height() != value ||
        d->mPrefCellMaxSize.height() != value)
    {
        d->mPrefCellMinSize.setHeight(value);
        d->mPrefCellMaxSize.setHeight(value);
        invalidate();
    }
}


/************************************************

 ************************************************/
void RazorGridLayout::setCellFixedWidth(int value)
{
    Q_D(RazorGridLayout);
    if (d->mPrefCellMinSize.width() != value ||
        d->mPrefCellMaxSize.width() != value)
    {
        d->mPrefCellMinSize.setWidth(value);
        d->mPrefCellMaxSize.setWidth(value);
        invalidate();
    }
}


/************************************************

 ************************************************/
QSize RazorGridLayout::sizeHint() const
{
    Q_D(const RazorGridLayout);

    if (!d->mIsValid)
        const_cast<RazorGridLayoutPrivate*>(d)->updateCache();

    return QSize(d->cols() * d->mCellSizeHint.width(),
                 d->rows() * d->mCellSizeHint.height());
}


/************************************************

 ************************************************/
void RazorGridLayout::setGeometry(const QRect &geometry)
{
    Q_D(RazorGridLayout);

    if (!d->mIsValid)
        d->updateCache();

    int y = geometry.top();
    int x = geometry.left();

    // For historical reasons QRect::right returns left() + width() - 1
    // and QRect::bottom() returns top() + height() - 1;
    // So we use left() + height() and top() + height()
    //
    // http://qt-project.org/doc/qt-4.8/qrect.html

    int maxX = geometry.left() + geometry.width();
    int maxY = geometry.top() + geometry.height();

    int itemWidth;
    if (d->mStretch.testFlag(StretchHoriz))
    {
        itemWidth = geometry.width() * 1.0 / d->cols();
        itemWidth = qMin(itemWidth, d->mCellMaxSize.width());
    }
    else
    {
        itemWidth = d->mCellSizeHint.width();
    }

    itemWidth = qBound(d->mPrefCellMinSize.width(), itemWidth, d->mPrefCellMaxSize.width());

    int itemHeight;
    if (d->mStretch.testFlag(StretchVert))
    {
        itemHeight = geometry.height() * 1.0 / d->rows();
        itemHeight = qMin(itemHeight, d->mCellMaxSize.height());
    }
    else
    {
        itemHeight = d->mCellSizeHint.height();
    }

    itemHeight = qBound(d->mPrefCellMinSize.height(), itemHeight, d->mPrefCellMaxSize.height());


#if 0
    qDebug() << "** RazorGridLayout::setGeometry *******************************";
    qDebug() << "Geometry:" << geometry;
    qDebug() << "CellSize:" << d->mCellSizeHint;
    qDebug() << "Constraints:" << "min" << d->mPrefCellMinSize << "max" << d->mPrefCellMaxSize;
    qDebug() << "Count" << count();
    qDebug() << "Cols:" << d->cols() << "(" << d->mColumnCount << ")";
    qDebug() << "Rows:" << d->rows() << "(" << d->mRowCount << ")";
    qDebug() << "Stretch:" << "h:" << (d->mStretch.testFlag(StretchHoriz)) << " v:" << (d->mStretch.testFlag(StretchVert));
    qDebug() << "Item:" << "h:" << itemHeight << " w:" << itemWidth;
#endif

    if (d->mDirection == LeftToRight)
    {       
        foreach(QLayoutItem *item, d->mItems)
        {
            if (!item->widget() || item->widget()->isHidden())
                continue;

            if (x + itemWidth > maxX)
            {
                x = geometry.left();
                if (d->mStretch.testFlag(StretchVert))
                    y += geometry.height() / d->rows();
                else
                    y += itemHeight;

            }

            item->setGeometry(QRect(x, y, itemWidth, itemHeight));
            x += itemWidth;
        }
    }
    else
    {
        foreach(QLayoutItem *item, d->mItems)
        {
            if (!item->widget() || item->widget()->isHidden())
                continue;

            if (y + itemHeight > maxY)
            {
                y = geometry.top();
                if (d->mStretch.testFlag(StretchHoriz))
                    x += geometry.width() / d->rows();
                else
                    x += itemWidth;

            }
            item->setGeometry(QRect(x, y, itemWidth, itemHeight));
            y += itemHeight;
        }
    }
}

