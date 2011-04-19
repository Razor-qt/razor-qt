/********************************************************************
  Copyright: 2010-2011 Alexander Sokoloff <sokoloff.a@gmail.ru>

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


#ifndef RAZORPANEL_H
#define RAZORPANEL_H

#include <QtGui/QFrame>
#include <QtCore/QString>

class QMenu;
class RazorPanelPrivate;

/*! \brief The RazorPanel class provides a single razor-panel.
 */
class RazorPanel : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(Position position READ position NOTIFY positionChanged)
    Q_ENUMS(Position)
    friend class RazorPanelPlugin;

public:
    enum Position{
        PositionBottom,
        PositionTop,
        PositionLeft,
        PositionRight
    };

    RazorPanel(QWidget *parent = 0);
    virtual ~RazorPanel();

    Position position() const ;
    bool isHorizontal() const { return position() == PositionBottom || position() == PositionTop; }

    virtual QMenu* popupMenu(QWidget *parent) const;

public slots:
    void show();

signals:
     void x11PropertyNotify(XEvent* event);
     void positionChanged();

protected:
    virtual void contextMenuEvent( QContextMenuEvent* event);

private:
    RazorPanelPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(RazorPanel)
};


#endif // RAZORPANEL_H
