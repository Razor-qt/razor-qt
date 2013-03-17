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


#ifndef RAZORGRIDLAYOUT_H
#define RAZORGRIDLAYOUT_H

#include <QtGui/QLayout>
#include <QtCore/QList>

class RazorGridLayoutPrivate;

/**
 The RazorGridLayout class lays out widgets in a grid.
 **/
class RazorGridLayout: public QLayout
{
    Q_OBJECT
public:
    /**
      This enum type is used to describe direction for this grid.
     **/
    enum Direction
    {
        LeftToRight,    ///< The items are first laid out horizontally and then vertically.
        TopToBottom     ///< The items are first laid out vertically and then horizontally.
    };

    /**
    This enum type is used to describe stretch. It contains one horizontal
    and one vertical flags that can be combined to produce the required effect.
    */
    enum StretchFlag
    {
        StretchHoriz    = 1,    ///< Justifies items in the available horizontal space
        StretchVert     = 2     ///< Justifies items in the available vertical space
    };
    Q_DECLARE_FLAGS(Stretch, StretchFlag)



    /**
    Constructs a new RazorGridLayout with parent widget, parent.
    The layout has one row and zero column initially, and will
    expand to left when new items are inserted.
    **/
    explicit RazorGridLayout(QWidget *parent = 0);

    /**
    Destroys the grid layout. The layout's widgets aren't destroyed.
     **/
    ~RazorGridLayout();

    void addItem(QLayoutItem *item);
    QLayoutItem *itemAt(int index) const;
    QLayoutItem *takeAt(int index);
    int count() const;
    void invalidate();

    QSize sizeHint() const;
    void setGeometry(const QRect &geometry);


    /**
    Returns the number of rows in this grid.
    **/
    int rowCount() const;

    /**
     Sets the number of rows in this grid. If value is 0, then rows
     count will calculated automatically when new items are inserted.

     In the most cases you should to set fixed number for one thing,
     or for rows, or for columns.

     \sa RazorGridLayout::setColumnCount
     **/
    void setRowCount(int value);


    /**
    Returns the number of columns in this grid.
    **/
    int columnCount() const;

    /**
     Sets the number of columns in this grid. If value is 0, then columns
     count will calculated automatically when new items are inserted.

     In the most cases you should to set fixed number for one thing,
     or for rows, or for columns.

     \sa RazorGridLayout::setRowCount
     **/
    void setColumnCount(int value);


    /**
    Returns the alignment of this grid.

    \sa  RazorGridLayout::Direction
    **/
    Direction direction() const;

    /**
     Sets the direction for this grid.

    \sa  RazorGridLayout::Direction
    **/
    void setDirection(Direction value);

    /**
    Returns the stretch flags of this grid.

    \sa  RazorGridLayout::StretchFlag
    **/
    Stretch stretch() const;

    /**
     Sets the stretch flags for this grid.

     \sa  RazorGridLayout::StretchFlag
     **/
    void setStretch(Stretch value);

    /**
      Moves the item at index position from to index position to.
     **/
    void moveItem(int from, int to);

private:
    RazorGridLayoutPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(RazorGridLayout)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(RazorGridLayout::Stretch)


#endif // RAZORGRIDLAYOUT_H
