/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
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


#include "clock.h"
#include <QtDebug>
#include <QGraphicsScene>
#include <QInputDialog>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QImage>
#include <QDateTime>
#include <QTimerEvent>

EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_CPP(Clock)


Clock::Clock(QGraphicsScene * scene, const QString & configId, ReadSettings * config)
    : DesktopWidgetPlugin(scene, configId, config)
{
//    QSettings * s = config->settings();
//    s->beginGroup(configId);
//    s->endGroup();

    mClockPix = new QPixmap(":/imgs/clock.png");
    mCenterPix = new  QPixmap(":/imgs/center.png");
    mHourPix = new  QPixmap(":/imgs/hour.png");
    mMinutePix = new  QPixmap(":/imgs/minute.png");
    mSecondsPix = new  QPixmap(":/imgs/seconds.png");
    mTimerId = startTimer(250);
}

Clock::~Clock()
{
}

    
QString Clock::instanceInfo()
{
    return tr("Clock");
}

void Clock::setSizeAndPosition(const QPointF & position, const QSizeF & size)
{
    setGeometry(position.x(), position.y(), mClockPix->width(), mClockPix->height());
}


void Clock::configure()
{
    save();
}

void Clock::save()
{
    QSettings *s = m_config->settings();
    s->beginGroup(m_configId);
    s->setValue("plugin", "analogclock");
    s->setValue("x", pos().x());
    s->setValue("y", pos().y());
    //s->setValue("w", boundingRect().width());
    //s->setValue("h", boundingRect().height());
    //s->setValue("text", toHtml());
    //s->setValue("color", defaultTextColor().name());
    s->endGroup();
}


void Clock::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == mTimerId)
        update();
}


void Clock::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QTime time = QTime::currentTime();
    int w = this->size().width();
    int h = this->size().height();

    painter->save();
    painter->translate(w/2, h/2);

    painter->drawPixmap(-mClockPix->width()/2.0, -mClockPix->height()/2.0, mClockPix->width(), mClockPix->height(), *mClockPix);

    painter->save();
    painter->rotate(30.0 * (time.hour() + time.minute() / 60.0));
    painter->drawPixmap(-mHourPix->width()/2.0, -mHourPix->height()+4 , mHourPix->width(), mHourPix->height(), *mHourPix);
    painter->restore();

    painter->save();
    painter->rotate(6.0 * (time.minute() + time.second() / 60.0));
    painter->drawPixmap(-mMinutePix->width()/2.0 , -mMinutePix->height()+3 , mMinutePix->width(), mMinutePix->height(), *mMinutePix);
    painter->restore();

    painter->save();
    painter->rotate(6.0 * (time.second() + time.msec() / 1000.0));
    painter->drawPixmap(-mSecondsPix->width()/2.0 , -mSecondsPix->height()+3 , mSecondsPix->width(), mSecondsPix->height(), *mSecondsPix);
    painter->restore();

    painter->drawPixmap(-mCenterPix->width()/2.0, -mCenterPix->height()/2.0, mCenterPix->width(), mCenterPix->height(), *mCenterPix);
    painter->restore();
}
