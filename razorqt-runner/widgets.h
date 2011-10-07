/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
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

#ifndef WIDGETS_H
#define WIDGETS_H


#include <QtGui/QWidget>
#include <QtGui/QComboBox>
#include <QtGui/QListView>
#include <QtGui/QStyledItemDelegate>

class QEvent;

class HorizSizeGrip: public QWidget
{
    Q_OBJECT
public:
    HorizSizeGrip(QWidget *parent = 0);

protected:
   void mouseMoveEvent(QMouseEvent *event);

};


class CommandComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit CommandComboBox(QWidget *parent = 0);
};


class CommandListView : public QListView
{
    Q_OBJECT
public:
    explicit CommandListView(QWidget *parent = 0);

signals:

public slots:

};


class HtmlDelegate : public QStyledItemDelegate
{
public:
    HtmlDelegate(const QSize iconSize, QObject* parent = 0):
        QStyledItemDelegate(parent),
        mIconSize(iconSize)
    {}
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
private:
    QSize mIconSize;
};


#endif // WIDGETS_H
