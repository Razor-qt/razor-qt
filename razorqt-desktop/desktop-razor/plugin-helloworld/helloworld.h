/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef HELLOWORLD_H
#define HELLOWORLD_H

#include <QGraphicsItem>

#include <razorqt/razorsettings.h>
#include <desktopwidgetplugin.h>


class HelloWorld : public DesktopWidgetPlugin, public QGraphicsTextItem
{
public:
    HelloWorld(QGraphicsScene * scene, const QString & configId, RazorSettings * config);
    ~HelloWorld();

    QString info();
    QString instanceInfo();

    void setSizeAndPosition(const QPointF & position, const QSizeF & size);
    void save();
    void configure();

    bool blockGlobalMenu() { return false; }

};

EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_H


#endif
