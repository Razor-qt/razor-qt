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

#ifndef RAZORPANELLAYOUT_H
#define RAZORPANELLAYOUT_H

#include <QtGui/QBoxLayout>
#include <QtCore/QList>

class RazorPanelLayout : public QBoxLayout
{
public:
    RazorPanelLayout(Direction dir, QWidget* parent=0);
    virtual ~RazorPanelLayout();

/*    void addItem(QLayoutItem * item);
    QSize sizeHint() const;

    QLayoutItem* itemAt(int index) const;
    QLayoutItem* takeAt(int index);
    int count() const;

    Qt::Orientations expandingDirections() const;
    QSize minimumSize() const;
    void setGeometry(const QRect& rect);

private:
    enum SizeType { MinimumSize, SizeHint };
    QSize calculateSize(SizeType sizeType) const;
    QList<QLayoutItem*> mItems;
*/
};

#endif // RAZORPANELLAYOUT_H
