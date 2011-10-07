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


#ifndef CLOCK_H
#define CLOCK_H

//#include <QGraphicsItem>
#include <QGraphicsWidget>
#include <razorqt/razorsettings.h>
#include <desktopwidgetplugin.h>

class QPixmap;

class Clock : public DesktopWidgetPlugin, public QGraphicsWidget
{
public:
    Clock(QGraphicsScene * scene, const QString & configId, RazorSettings * config);
    ~Clock();

    QString instanceInfo();

    void setSizeAndPosition(const QPointF & position, const QSizeF & size);
    void save();
    void configure();

    bool blockGlobalMenu() { return false; }
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget=0);

protected:
    virtual void timerEvent(QTimerEvent* event);

private:
    void startClock();

    QPixmap* mClockPix;
    QPixmap* mCenterPix;
    QPixmap* mHourPix;
    QPixmap* mMinutePix;
    QPixmap* mSecondsPix;
    int mTimerId;
    bool mShowSeconds;
};

EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_H


#endif
