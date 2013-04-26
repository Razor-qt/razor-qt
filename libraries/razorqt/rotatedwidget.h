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

#ifndef ROTATED_WIDGET_H
#define ROTATED_WIDGET_H

#include <QWidget>

class RotatedWidget: public QWidget
{
    Q_OBJECT

    Q_PROPERTY(Qt::Corner origin READ origin WRITE setOrigin)

public:
    explicit RotatedWidget(QWidget &content, QWidget* parent = 0, Qt::WindowFlags f = 0);

    Qt::Corner origin() const;
    void setOrigin(Qt::Corner);

    QWidget * content() const;

    void adjustContentSize();

    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;

    QSize adjustedSize(QSize) const;
    QPoint adjustedPoint(QPoint) const;

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    QWidget *mContent;
    Qt::Corner mOrigin;
};

#endif
