/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL3+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *   Petr Vanek <petr@scribus.info>
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

/*
    This plugin uses images from the Qt clocks example.
    http://doc.qt.nokia.com/latest/declarative-toys-clocks.html
 */

#include "clock.h"
#include <QtDebug>
#include <QGraphicsScene>
//#include <QInputDialog>
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include <QTime>
#include <QTimerEvent>
#include "configuredialog.h"

EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_CPP(Clock)


Clock::Clock(QGraphicsScene * scene, const QString & configId, RazorSettings * config)
    : DesktopWidgetPlugin(scene, configId, config)
{
    config->beginGroup(configId);
    mShowSeconds = config->value("showseconds", false).toBool();
    config->endGroup();

    mClockPix = new QPixmap(":/imgs/clock.png");
    mCenterPix = new  QPixmap(":/imgs/center.png");
    mHourPix = new  QPixmap(":/imgs/hour.png");
    mMinutePix = new  QPixmap(":/imgs/minute.png");
    mSecondsPix = new  QPixmap(":/imgs/seconds.png");

    mTimerId = 0;
    startClock();
}


Clock::~Clock()
{
    //killTimer(mTimerId);
    delete mClockPix;
    delete mCenterPix;
    delete mHourPix;
    delete mMinutePix;
    delete mSecondsPix;
}


void Clock::startClock()
{
    if (mTimerId)
        killTimer(mTimerId);

    if (mShowSeconds)
        mTimerId = startTimer(1000);
    else
        mTimerId = startTimer(10000);
}


QString Clock::instanceInfo()
{
    return QObject::tr("Clock");
}


void Clock::setSizeAndPosition(const QPointF & position, const QSizeF & size)
{
    int x = position.x();
    int y=position.y();

    if (x<0)
    {
        x = QApplication::desktop()->availableGeometry().width()- mClockPix->width() - (-position.x());
    }

    if (y<0)
    {
        y = QApplication::desktop()->availableGeometry().height() - mClockPix->height() - (-position.y());
    }
    setGeometry(x, y, mClockPix->width(), mClockPix->height());
}


void Clock::configure()
{
    ConfigureDialog dlg;
    dlg.setShowSecondsHand(mShowSeconds);
    if (dlg.exec())
    {
        mShowSeconds = dlg.showSecondsHand();
        startClock();
        update();
    }
    save();
}


void Clock::save()
{
    m_config->beginGroup(m_configId);
    m_config->setValue("plugin", "analogclock");
    m_config->setValue("x", pos().x());
    m_config->setValue("y", pos().y());
    m_config->setValue("showseconds", mShowSeconds);
    m_config->endGroup();
}


void Clock::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == mTimerId)
        update();
    else
        QGraphicsWidget::timerEvent(event);
}


void Clock::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QTime time = QTime::currentTime();
    int w = this->size().width();
    int h = this->size().height();

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    //painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    //painter->setRenderHint(QPainter::HighQualityAntialiasing , true);

    painter->translate(w/2.0, h/2.0);

    painter->drawPixmap(-mClockPix->width()/2.0, -mClockPix->height()/2.0, mClockPix->width(), mClockPix->height(), *mClockPix);

    painter->save();
    painter->rotate(30.0 * (time.hour() + time.minute() / 60.0));
    painter->drawPixmap(-mHourPix->width()/2.0, -mHourPix->height()+4 , mHourPix->width(), mHourPix->height(), *mHourPix);
    painter->restore();

    qreal a = 6.0 * time.minute();
    if (mShowSeconds)
        a += time.second() * 0.1;

    painter->save();
    painter->rotate(a);
    painter->drawPixmap(-mMinutePix->width()/2.0 , -mMinutePix->height()+3 , mMinutePix->width(), mMinutePix->height(), *mMinutePix);
    painter->restore();

    if (mShowSeconds)
    {
        painter->save();
        painter->rotate(6.0 * (time.second() /*+ time.msec() / 1000.0*/));
        painter->setPen(QColor(211, 48, 47));
        painter->drawLine(0, 0, 0, -80);
        painter->restore();
    }

    painter->drawPixmap(-mCenterPix->width()/2.0, -mCenterPix->height()/2.0, mCenterPix->width(), mCenterPix->height(), *mCenterPix);
    painter->restore();
}
