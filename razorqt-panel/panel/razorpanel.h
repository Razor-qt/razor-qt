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
//#include <QMetaType>
#include "irazorpanel.h"

class QMenu;
class RazorPanelPrivate;
class Plugin;


/*! \brief The RazorPanel class provides a single razor-panel.
 */
class RazorPanel : public QFrame, public IRazorPanel
{
    Q_OBJECT

    //Q_ENUMS(Position)
    //Q_PROPERTY(IRazorPanel::Position position READ position)// NOTIFY positionChanged)
public:
    enum Alignment {
        AlignmentLeft   = -1,
        AlignmentCenter =  0,
        AlignmentRight  =  1
    };

    RazorPanel(QWidget *parent = 0);
    virtual ~RazorPanel();

    void showPopupMenu(Plugin *plugin = 0);
    void x11EventFilter(XEvent* event);

    // IRazorPanel .........................
    Position position() const;
    QRect globalGometry() const;
    QRect calculatePopupWindowPos(const IRazorPanelPlugin *plugin, const QSize &windowSize) const;

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

//Q_DECLARE_METATYPE(IRazorPanel::Position)

#endif // RAZORPANEL_H
