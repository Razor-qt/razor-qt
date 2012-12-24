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


#include "razortaskbarlayout.h"
#include <math.h>
#include <QDebug>
#include <QWidget>

RazorTaskBarLayout::RazorTaskBarLayout(QWidget *parent):
    QLayout(parent),
    mLineCount(1),
    mHorizDirection(LeftToRight)
{

}

RazorTaskBarLayout::~RazorTaskBarLayout()
{
    qDeleteAll(mItems);
}

void RazorTaskBarLayout::addItem(QLayoutItem *item)
{
    mItems.append(item);
}

QLayoutItem *RazorTaskBarLayout::itemAt(int index) const
{
    if (index < 0 || index >= mItems.count())
        return 0;

    return mItems.at(index);
}

QLayoutItem *RazorTaskBarLayout::takeAt(int index)
{
    if (index < 0 || index >= mItems.count())
        return 0;

    QLayoutItem *item = mItems.takeAt(index);
    return item;
}

void RazorTaskBarLayout::setLineCount(int value)
{
    if (value < 1)
        value = 1;
    if (mLineCount != value)
    {
        mLineCount = value;
        invalidate();
    }
}

void RazorTaskBarLayout::setIsHorizontal(bool value)
{
    if (mHoriz != value)
    {
        mHoriz = value;
        invalidate();
    }
}

void RazorTaskBarLayout::setHorizontalDirection(RazorTaskBarLayout::HorizontalDirection value)
{
    if (mHorizDirection != value)
    {
        mHorizDirection = value;
        invalidate();
    }
}

QSize RazorTaskBarLayout::sizeHint() const
{
    int maxW = 0;
    int maxH = 0;
    int cnt = 0;

    for (int i=0; i<mItems.count(); ++i)
    {
        QSize sz = itemAt(i)->sizeHint();
        maxH = qMax(maxH, sz.height());
        maxW = qMax(maxW, sz.width());

        if (itemAt(i)->widget() && itemAt(i)->widget()->isVisible())
            cnt++;
    }


    if (mHoriz)
    {
        int btnsPerLine = ceil(mItems.count() * 1.0 / mLineCount);
        return QSize(btnsPerLine * maxW, mLineCount * maxH);
    }
    else
    {
        return QSize(maxW, cnt * maxH);
    }
}

void RazorTaskBarLayout::setGeometry(const QRect &geometry)
{
    int cnt=0;
    for (int i=0; i<mItems.count(); ++i)
    {
        if (itemAt(i)->widget() && itemAt(i)->widget()->isVisible())
            cnt++;
    }


    if (mHoriz)
    {
        int btnsPerLine = ceil(cnt * 1.0 / mLineCount);
        int itemWidth = geometry.width() * 1.0 / btnsPerLine;
        int itemHeight = geometry.height() * 1.0 / mLineCount;

        int y = geometry.top();
        int x = geometry.left();

        if (mHorizDirection == LeftToRight)
        {
            for (int i=0; i<mItems.count(); ++i)
            {
                QLayoutItem *item = itemAt(i);
                if (!item->widget() || !item->widget()->isVisible())
                    continue;

                int w = qMin(item->maximumSize().width(),  itemWidth);
                int h = qMin(item->maximumSize().height(), itemHeight);
                if (x + w > geometry.right())
                {
                    x = geometry.left();
                    y += h;
                }
                item->setGeometry(QRect(x, y, w, h));
                x += w;
            }
        }
        else
        {
            for (int i=0; i<mItems.count(); ++i)
            {
                QLayoutItem *item = itemAt(i);
                if (!item->widget() || !item->widget()->isVisible())
                    continue;

                int w = qMin(item->maximumSize().width(),  itemWidth);
                int h = qMin(item->maximumSize().height(), itemHeight);
                if (y + h > geometry.height())
                {
                    y = geometry.top();
                    x += w;
                }
                item->setGeometry(QRect(x, y, w, h));
                y += h;
            }
        }
    }
    else
    {
        int itemWidth = geometry.width();
        //int itemHeight =

        int y = geometry.top();
        int x = geometry.left();

        for (int i=0; i<mItems.count(); ++i)
        {
            QLayoutItem *item = itemAt(i);
            if (!item->widget() || !item->widget()->isVisible())
                continue;

            int w = qMin(item->maximumSize().width(),  itemWidth);
            int h = item->sizeHint().height();// qMin(item->maximumSize().height(), itemHeight);
            qDebug() << h;
            item->setGeometry(QRect(x, y, w, h));
            y += h;
        }
    }


#if 0
        int rowLen = ceil(cnt * 1.0 / mLineCount);
        int itemWidth = geometry.width() * 1.0 / rowLen;
        int itemHeight = geometry.height() * 1.0 / mLineCount;

        qDebug() << "*****************";
        qDebug() << "RowLen" << rowLen << "= ceil" << cnt << "/" << mLineCount;
        qDebug() << "itemWidth" << itemWidth << "=" << geometry.width()  << "/" << rowLen;
        qDebug() << "itemHeight" << itemHeight << "=" << geometry.height() << mLineCount;
        int y = geometry.top();
        int x = geometry.left();

        for (int i=0; i<mItems.count(); ++i)
        {
            if (itemAt(i)->widget() && itemAt(i)->widget()->isVisible())
            {
                QLayoutItem *item = itemAt(i);
                int w = qMin(item->maximumSize().width(),  itemWidth);
                int h = qMin(item->maximumSize().height(), itemHeight);
                if (y + h > geometry.height())
                {
                    //qDebug() << geometry.top() << geometry.bottom() << geometry.height() << "[" << y << h << "]";
                    y = geometry.top();
                    x += w;
                }
                qDebug() << " *"<< i << QRect(x, y, w, h) << QRect(x, y, w, h).bottom();
                item->setGeometry(QRect(x, y, w, h));
                y += h;
            }
        }


        for (int i=0; i<mItems.count(); ++i)
        {
            QLayoutItem *item = itemAt(i);
            int w = qMin(item->maximumSize().width(),  itemWidth);
            int h = qMin(item->maximumSize().height(), itemHeight);
            if (x + w > geometry.right())
            {
                x = geometry.left();
                y += itemHeight;
            }
            item->setGeometry(QRect(x, y, w, h));
            x += w;
        }
#endif

}





