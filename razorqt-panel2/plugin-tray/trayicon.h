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

#ifndef TRAYICON_H
#define TRAYICON_H

#include <QtCore/QObject>
#include <QtGui/QFrame>
#include <QtCore/QList>

#include <X11/X.h>
#include <X11/extensions/Xdamage.h>

#define TRAY_ICON_SIZE_DEFAULT 24

class QWidget;
class RazorPanel;

class TrayIcon: public QFrame
{
    Q_OBJECT
    Q_PROPERTY(int iconSize READ iconSize WRITE setIconSize)

public:
    TrayIcon(Window iconId, QWidget* parent);
    virtual ~TrayIcon();

    Window iconId() { return mIconId; }
    Window windowId() { return mWindowId; }

    bool isValid() const { return mValid; }

    int iconSize() const { return mIconSize; }
    void setIconSize(int iconSize);

    QSize sizeHint() const;

protected:
    bool event(QEvent *event);
    void draw(QPaintEvent* event);

private:
    bool init();
    QRect iconGeometry();
    Window mIconId;
    Window mWindowId;
    bool mValid;
    int mIconSize;
    Damage mDamage;
};

#endif // TRAYICON_H
