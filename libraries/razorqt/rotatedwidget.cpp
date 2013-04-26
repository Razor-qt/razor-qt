/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2013 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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


#include "rotatedwidget.h"

#include <QPainter>
#include <QImage>


RotatedWidget::RotatedWidget(QWidget &content, QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , mContent(&content)
    , mOrigin(Qt::TopLeftCorner)
{
    mContent->setParent(this);
    mContent->hide();
}

Qt::Corner RotatedWidget::origin() const
{
    return mOrigin;
}

void RotatedWidget::setOrigin(Qt::Corner newOrigin)
{
    if (mOrigin != newOrigin)
    {
        mOrigin = newOrigin;
        adjustContentSize();
        update();
    }
}

QWidget * RotatedWidget::content() const
{
    return mContent;
}

void RotatedWidget::adjustContentSize()
{
    mContent->adjustSize();

    QSize before = size();

    adjustSize();

    if (before != size())
        updateGeometry();
}

QSize RotatedWidget::minimumSizeHint() const
{
    QSize result = mContent->minimumSizeHint();

    switch (mOrigin)
    {
    case Qt::TopLeftCorner:
    case Qt::BottomRightCorner:
        break;

    case Qt::TopRightCorner:
    case Qt::BottomLeftCorner:
        result.transpose();
        break;
    }

    return result;
}

QSize RotatedWidget::sizeHint() const
{
    QSize result = mContent->sizeHint();

    switch (mOrigin)
    {
    case Qt::TopLeftCorner:
    case Qt::BottomRightCorner:
        break;

    case Qt::TopRightCorner:
    case Qt::BottomLeftCorner:
        result.transpose();
        break;
    }

    return result;
}

void RotatedWidget::paintEvent(QPaintEvent */*event*/)
{
    QSize sz = mContent->size();
    QImage image(sz, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(this);
    mContent->render(&image, QPoint(), QRegion(), RenderFlags(DrawChildren));

    QTransform transform;
    QPointF originPoint;
    switch (mOrigin)
    {
    case Qt::TopLeftCorner:
//        transform.rotate(0.0);
//        originPoint = QPointF(0.0, 0.0);
        break;

    case Qt::TopRightCorner:
        transform.rotate(90.0);
        originPoint = QPointF(0.0, -sz.height());
        break;

    case Qt::BottomRightCorner:
        transform.rotate(180.0);
        originPoint = QPointF(-sz.width(), -sz.height());
        break;

    case Qt::BottomLeftCorner:
        transform.rotate(270.0);
        originPoint = QPointF(-sz.width(), 0.0);
        break;
    }

    painter.setTransform(transform);
    painter.drawImage(originPoint, image);
}
