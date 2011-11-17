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
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)

public:
    TrayIcon(Window iconId, QWidget* parent);
    virtual ~TrayIcon();

    Window iconId() { return mIconId; }
    Window windowId() { return mWindowId; }

    bool isValid() const { return mValid; }

    QSize iconSize() const { return mIconSize; }
    void setIconSize(QSize iconSize);

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
    QSize mIconSize;
    Damage mDamage;

    static bool isXCompositeAvailable();
};

#endif // TRAYICON_H
