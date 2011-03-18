/********************************************************************
  Copyright: 2011 Alexander Sokoloff <sokoloff.a@gmail.ru>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*********************************************************************/

#include "razorpanellayout.h"
#include <QtCore/QSize>
#include <QDebug>


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
//void RazorPanelLayout::addItem(QLayoutItem * item)
//{
//    qDebug() << "AddItem";
//    mItems.append(item);
//}


/************************************************

 ************************************************/
//QSize RazorPanelLayout::sizeHint() const
//{
//    return calculateSize(SizeHint);
//}


/************************************************

 ************************************************/
//QLayoutItem* RazorPanelLayout::itemAt(int index) const
//{
//    if (index<mItems.count())
//        return mItems.at(index);
//    else
//        return 0;
//}


/************************************************

 ************************************************/
//QLayoutItem* RazorPanelLayout::takeAt(int index)
//{
//    if (index<mItems.count())
//        return mItems.takeAt(index);
//    else
//        return 0;
//}


/************************************************

 ************************************************/
//int RazorPanelLayout::count() const
//{
//    return mItems.size();
//}


/************************************************

 ************************************************/
//Qt::Orientations RazorPanelLayout::expandingDirections() const
//{
//    return Qt::Horizontal | Qt::Vertical;
//}


/************************************************

 ************************************************/
//QSize RazorPanelLayout::minimumSize() const
//{
//    return calculateSize(MinimumSize);
//}


/************************************************

 ************************************************/
//void RazorPanelLayout::setGeometry(const QRect& rect)
//{
//    int leftPos = 0;

//    //foreach

//}


/************************************************

 ************************************************/
//QSize RazorPanelLayout::calculateSize(SizeType sizeType) const
//{
//    QSize totalSize;

//    foreach (QLayoutItem* item, mItems)
//    {
//        QSize itemSize;
//        switch (sizeType)
//        {
//            case MinimumSize:
//                itemSize = item->minimumSize();
//                break;

//            case SizeHint:
//                itemSize = item->sizeHint();
//                break;
//        }

//        totalSize.rwidth() += itemSize.width();
//        totalSize.rheight() = 32;//qMin(totalSize.height(), itemSize.height());
//    }

//    return totalSize;
//}
