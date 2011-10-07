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

#include "widgets.h"

#include <QtGui/QMouseEvent>
#include <QtCore/QDebug>


#include <QtGui/QTextDocument>
#include <QtGui/QPainter>
#include <QtGui/QAbstractTextDocumentLayout>



/************************************************

 ************************************************/
HorizSizeGrip::HorizSizeGrip(QWidget *parent):
    QWidget(parent)
{
}


/************************************************

 ************************************************/
void HorizSizeGrip::mouseMoveEvent(QMouseEvent *event)
{
    if (!parentWidget())
        return;

    QWidget *parent = parentWidget();
    while (parent->parentWidget() && !parent->isWindow())
    {
        parent = parent->parentWidget();
    }

    QRect rect = parent->geometry();
    int delta = event->globalPos().x() - rect.center().x();

    bool isLeft = pos().x() < parent->size().width() - geometry().right();
    if (isLeft)
    {
        rect.setLeft(event->globalPos().x());
        rect.setRight(parent->geometry().center().x() - delta);
    }
    else
    {
        rect.setLeft(parent->geometry().center().x() - delta);
        rect.setRight(event->globalPos().x());
    }

    if (rect.width() < parent->minimumWidth() ||
        rect.width() > parent->maximumWidth())
        return;

    parent->setGeometry(rect);
}


/************************************************

 ************************************************/
CommandComboBox::CommandComboBox(QWidget *parent) :
    QComboBox(parent)
{

}



/************************************************

 ************************************************/
void HtmlDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!index.isValid())
        return;

    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    painter->save();

    QTextDocument doc;
    doc.setHtml(options.text);
    QIcon icon = options.icon;

    options.text = "";
    options.icon = QIcon();
    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

    // Draw icon ................................
    QSize iconSize = icon.actualSize(mIconSize);
    painter->translate(options.rect.left(), options.rect.top());
    QRect iconRect = QRect(4, 4, iconSize.width(), iconSize.height());

    icon.paint(painter, iconRect);

    doc.setTextWidth(options.rect.width() - mIconSize.width() - 10);

    // shift text right to make icon visible
    painter->translate(mIconSize.width() + 8, 0);
    QRect clip(0, 0, options.rect.width()-mIconSize.width() - 10, options.rect.height());

    painter->setClipRect(clip);

    QAbstractTextDocumentLayout::PaintContext ctx;
    if (option.state & QStyle::State_Selected)
    {
        QPalette::ColorGroup colorGroup =  (option.state & QStyle::State_Active) ? QPalette::Active : QPalette::Inactive;
        ctx.palette.setColor(QPalette::Text, option.palette.color(colorGroup, QPalette::HighlightedText));
    }

    ctx.clip = clip;
    doc.documentLayout()->draw(painter, ctx);

    painter->restore();
}



/************************************************

 ************************************************/
CommandListView::CommandListView(QWidget *parent) :
    QListView(parent)
{

}

