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


#ifndef RAZORPANEL_H
#define RAZORPANEL_H

#include <QtGui/QFrame>
#include <QtCore/QString>
#include <QtGui/QBoxLayout>

class QMenu;
class RazorPanelPrivate;
class RazorPanelPlugin;

/*! \brief The RazorPanel class provides a single razor-panel.
 */
class RazorPanel : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(Position position READ position NOTIFY positionChanged)
    Q_PROPERTY(Orientation orientation READ orientation)
    Q_ENUMS(Position)
    Q_ENUMS(Orientation)
    friend class RazorPanelPlugin;

public:
    enum Position{
        PositionBottom,
        PositionTop,
        PositionLeft,
        PositionRight
    };

    enum Alignment {
        AlignmentLeft   = -1,
        AlignmentCenter =  0,
        AlignmentRight  =  1
    };

    enum Orientation {
        Horizontal,
        Vertical
    };

    RazorPanel(QWidget *parent = 0);
    virtual ~RazorPanel();

    Position position() const ;
    bool isHorizontal() const { return position() == PositionBottom || position() == PositionTop; }
    Orientation orientation() const;

    void showPopupMenu(RazorPanelPlugin *plugin = 0);
    void x11EventFilter(XEvent* event);

public slots:
    void show();

signals:
    void positionChanged();
    void layoutDirectionChanged(QBoxLayout::Direction direction);
    void panelRealigned();
    
protected:
    virtual bool event(QEvent* e);

private:
    RazorPanelPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(RazorPanel)
};


#endif // RAZORPANEL_H
