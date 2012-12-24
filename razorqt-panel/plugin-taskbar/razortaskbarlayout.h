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


#ifndef RAZORTASKBARLAYOUT_H
#define RAZORTASKBARLAYOUT_H

#include <QtGui/QLayout>
#include <QtCore/QList>

class RazorTaskBarLayout : public QLayout
{
    Q_OBJECT
public:
    enum HorizontalDirection
    {
        LeftToRight,
        TopToBottom

    };

    explicit RazorTaskBarLayout(QWidget *parent = 0);
    ~RazorTaskBarLayout();

    void addItem(QLayoutItem *item);
    QLayoutItem *itemAt(int index) const;
    QLayoutItem *takeAt(int index);
    int count() const { return mItems.count(); }

    QSize sizeHint() const;
    void setGeometry(const QRect &geometry);

    int lineCount() const { return mLineCount; }
    void setLineCount(int value);

    bool isHorizontal() const { return mHoriz; }
    void setIsHorizontal(bool value);

    HorizontalDirection horizontalDirection() const { return mHorizDirection; }
    void setHorizontalDirection(HorizontalDirection value);

signals:
    
public slots:
private:
    QList<QLayoutItem*> mItems;
    int mLineCount;
    bool mHoriz;
    HorizontalDirection mHorizDirection;
};

#endif // RAZORTASKBARLAYOUT_H
