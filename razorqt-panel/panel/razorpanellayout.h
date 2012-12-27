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


#ifndef RAZORPANELLAYOUT_H
#define RAZORPANELLAYOUT_H

#include <QtGui/QLayout>
#include <QtCore/QList>
#include <QtGui/QWidget>
#include <QtGui/QLayoutItem>
#include "irazorpanel.h"


class MoveInfo;
class QMouseEvent;
class QEvent;

class Plugin;
class LayoutItemGrid;

class RazorPanelLayout : public QLayout
{
    Q_OBJECT
public:
    explicit RazorPanelLayout(QWidget *parent);
    ~RazorPanelLayout();

    void addItem(QLayoutItem *item);
    QLayoutItem *itemAt(int index) const;
    QLayoutItem *takeAt(int index);
    int count() const;
    void moveItem(int from, int to, bool withAnimation=false);

    QSize sizeHint() const;
    //QSize minimumSize() const;
    void setGeometry(const QRect &geometry);

    bool isHorizontal() const;

    void invalidate();

    int lineCount() const;
    void setLineCount(int value);

    int lineSize() const;
    void setLineSize(int value);

    IRazorPanel::Position position() const { return mPosition; }
    void setPosition(IRazorPanel::Position value);

    static bool itemIsSeparate(QLayoutItem *item);
signals:
    void pluginMoved();

public slots:
    void startMovePlugin();
    void finishMovePlugin();

private:
    mutable QSize mMinPluginSize;
    LayoutItemGrid *mLeftGrid;
    LayoutItemGrid *mRightGrid;
    IRazorPanel::Position mPosition;
    bool mAnimate;


    void setGeometryHoriz(const QRect &geometry);
    void setGeometryVert(const QRect &geometry);
    void globalIndexToLocal(int index, LayoutItemGrid **grid, int *gridIndex);
    void globalIndexToLocal(int index, LayoutItemGrid **grid, int *gridIndex) const;

    void setItemGeometry(QLayoutItem *item, const QRect &geometry, bool withAnimation);
};

#endif // RAZORPANELLAYOUT_H
