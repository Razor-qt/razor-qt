/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Christopher "VdoP" Regali
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *   Maciej Płaza <plaza.maciej@gmail.com>
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


#ifndef RAZORCLOCK_CPP
#define RAZORCLOCK_CPP

#include "razorclock.h"
#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtGui/QCalendarWidget>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QMouseEvent>
#include <QtCore/QPoint>
#include <QtCore/QSettings>
#include <QtCore/QRect>

/**
 * @file razorclock.cpp
 * @brief implements Razorclock and Razorclockgui
 * @author Christopher "VdoP" Regali
 * @author Kuzma Shapran
 */
EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorClock)

/**
 * @brief constructor
 */
RazorClock::RazorClock(const RazorPanelPluginStartInfo* startInfo, QWidget* parent):
        RazorPanelPlugin(startInfo, parent),
        content(new QWidget(this)),
        calendarDialog(0)
{
    setObjectName("Clock");
    clockFormat = "hh:mm";

    fakeThemedLabel = new ClockLabel(this);
    fakeThemedLabel->setVisible(false);

    timeLabel = new QLabel(this);
    dateLabel = new QLabel(this);
    QVBoxLayout *contentLayout = new QVBoxLayout(content);
    contentLayout->addWidget(timeLabel, 0, Qt::AlignCenter);
    contentLayout->addWidget(dateLabel, 0, Qt::AlignCenter);
    content->setLayout(contentLayout);
    addWidget(content);

    contentLayout->setContentsMargins(0, 0, 0, 0);
    this->layout()->setContentsMargins(2, 0, 2, 0);

    contentLayout->setSpacing(1);
    this->layout()->setSpacing(0);

    timeLabel->setAlignment(Qt::AlignCenter);
    dateLabel->setAlignment(Qt::AlignCenter);
    contentLayout->setAlignment(Qt::AlignCenter);
    this->layout()->setAlignment(Qt::AlignCenter);

    timeLabel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    dateLabel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    content->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    this->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));

    settingsChanged();
    connect(fakeThemedLabel, SIGNAL(fontChanged()), this, SLOT(fontChanged()));

    clocktimer = new QTimer(this);
    connect (clocktimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    clocktimer->start(1000);
}

/**
 * @brief updates the time
 * Color and font settings can be configured in Qt CSS
 */
void RazorClock::updateTime()
{
    if (dateOnNewLine && showDate)
    {
        timeLabel->setText(QDateTime::currentDateTime().toString(timeFormat));
        dateLabel->setText(QDateTime::currentDateTime().toString(dateFormat));
    }
    else
    {
        timeLabel->setText(QDateTime::currentDateTime().toString(clockFormat));
    }
}

/**
 * @brief destructor
 */
RazorClock::~RazorClock()
{
}

void RazorClock::settingsChanged()
{
    if (QLocale::system().timeFormat(QLocale::ShortFormat).toUpper().contains("AP") == true)
    {
        timeFormat = settings().value("timeFormat", "h:mm AP").toString();
    }
    else
    {
        timeFormat = settings().value("timeFormat", "HH:mm").toString();
    }

    dateFormat = settings().value("dateFormat", Qt::SystemLocaleShortDate).toString();

    dateOnNewLine = settings().value("dateOnNewLine", true).toBool();
    showDate = settings().value("showDate", false).toBool();

    clockFormat = timeFormat;
    if (showDate)
    {
        if (!dateOnNewLine)
        {
            clockFormat.append(" ");
            clockFormat += dateFormat;
        }
    }

    fontChanged();

    dateLabel->setVisible(showDate && dateOnNewLine);
    updateTime();
}

void RazorClock::fontChanged()
{
    if (settings().value("useThemeFonts", true).toBool())
    {
        timeLabel->setFont(fakeThemedLabel->font());
        dateLabel->setFont(fakeThemedLabel->font());
    }
    else
    {
        {
            QFont font(timeLabel->font());
            timeLabel->setFont(QFont(
                settings().value("timeFont/family", font.family()).toString(),
                settings().value("timeFont/pointSize", font.pointSize()).toInt(),
                settings().value("timeFont/weight", font.weight()).toInt(),
                settings().value("timeFont/italic", font.italic()).toBool() ));
            font = timeLabel->font();
        }

        {
            QFont font(dateLabel->font());
            dateLabel->setFont(QFont(
                settings().value("dateFont/family", font.family()).toString(),
                settings().value("dateFont/pointSize", font.pointSize()).toInt(),
                settings().value("dateFont/weight", font.weight()).toInt(),
                settings().value("dateFont/italic", font.italic()).toBool() ));
            font = dateLabel->font();
        }
    }

    updateMinWidth();
}


static QDate getMaxDate(const QFontMetrics &metrics, const QString &format)
{
    QDate d(QDate::currentDate().year(), 1, 1);
    QDateTime dt(d);
    QDate res;

    int maxWidth = 0;
    while (dt.date().year() == d.year())
    {
        int w = metrics.boundingRect(dt.toString(format)).width();
        //qDebug() << "*" << dt.toString(format) << w;
        if (w > maxWidth)
        {
            res = dt.date();
            maxWidth = w;
        }
        dt = dt.addDays(1);
    }

    //qDebug() << "Max date:" << res.toString(format);
    return res;
}


static QTime getMaxTime(const QFontMetrics &metrics, const QString &format)
{
    int maxMinSec = 0;
    for (int width=0, i=0; i<60; ++i)
    {
        int w = metrics.boundingRect(QString("%1").arg(i, 2, 10, QChar('0'))).width();
        if (w > width)
        {
            maxMinSec = i;
            width = w;
        }
    }

    QTime res;
    QDateTime dt(QDate(1, 1, 1), QTime(0, maxMinSec, maxMinSec));

    int maxWidth = 0;
    while (dt.date().day() == 1)
    {
        int w = metrics.boundingRect(dt.toString(format)).width();
        //qDebug() << "*" << dt.toString(format) << w;
        if (w > maxWidth)
        {
            res = dt.time();
            maxWidth = w;
        }
        dt = dt.addSecs(3600);
    }

    //qDebug() << "Max time:" << res.toString();
    return res;
}

/************************************************
  Issue #18: Panel clock plugin changes your size
 ************************************************/
void RazorClock::updateMinWidth()
{
    QFontMetrics timeLabelMetrics(timeLabel->font());
    QFontMetrics dateLabelMetrics(dateLabel->font());
    QDate maxDate = getMaxDate((dateOnNewLine && showDate) ? dateLabelMetrics : timeLabelMetrics, dateFormat);
    QTime maxTime = getMaxTime(timeLabelMetrics, timeFormat);
    QDateTime dt(maxDate, maxTime);

    //qDebug() << "T:" << metrics.boundingRect(dt.toString(timeFormat)).width();
    //qDebug() << "C:" << metrics.boundingRect(QTime::currentTime().toString(timeFormat)).width() << QTime::currentTime().toString(timeFormat);
    //qDebug() << "D:" << metrics.boundingRect(dt.toString(dateFormat)).width();

    int width;
    int height;
    if (dateOnNewLine && showDate)
    {
        QRect rect1(timeLabelMetrics.boundingRect(dt.toString(timeFormat)));
        timeLabel->setMinimumSize(rect1.size());
        QRect rect2(dateLabelMetrics.boundingRect(dt.toString(dateFormat)));
        dateLabel->setMinimumSize(rect2.size());
        width = qMax(rect1.width(), rect2.width());
        height = rect1.height() + rect2.height();
//        qDebug() << "RazorClock Recalc size" << width << height << dt.toString(timeFormat) << dt.toString(dateFormat);
    }
    else
    {
        QRect rect(timeLabelMetrics.boundingRect(dt.toString(clockFormat)));
        timeLabel->setMinimumSize(rect.size());
        dateLabel->setMinimumSize(0, 0);
        width = rect.width();
        height = rect.height();
//        qDebug() << "RazorClock Recalc size" << width << height << dt.toString(clockFormat);
    }


    content->setMinimumSize(width, height);
}


void RazorClock::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return;

    if (!calendarDialog)
    {
        calendarDialog = new QDialog(this);
        //calendarDialog->setAttribute(Qt::WA_DeleteOnClose, true);
        calendarDialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        calendarDialog->setLayout(new QHBoxLayout(calendarDialog));
        calendarDialog->layout()->setMargin(1);

        QCalendarWidget* cal = new QCalendarWidget(calendarDialog);
        calendarDialog->layout()->addWidget(cal);
        calendarDialog->adjustSize();

        int x, y;
        RazorPanel::Position pos = panel()->position();
        QRect panelRect = panel()->geometry();
        int calHeight = calendarDialog->height();
        int calWidth = calendarDialog->width();

        if (pos == RazorPanel::PositionBottom || pos == RazorPanel::PositionTop)
        {
            int rightMax = panelRect.topRight().x() - calWidth + 1;
            x = panel()->mapToGlobal(this->geometry().topLeft()).x();
            if (x > rightMax)
                x = rightMax;
            if (pos == RazorPanel::PositionBottom)
                y = panelRect.top() - calHeight;
            else
                y = panelRect.bottom() + 1;
        }
        else // PositionRight or PositionLeft
        {
            int bottomMax = panelRect.bottomRight().y() - calHeight + 1;
            y = panel()->mapToGlobal(this->geometry().topRight()).y();
            if (y > bottomMax)
                y = bottomMax;
            if (pos == RazorPanel::PositionRight)
                x = panelRect.left() - calWidth;
            else
                x = panelRect.right() + 1;
        }

        calendarDialog->move(QPoint(x, y));
        calendarDialog->show();
    }
    else
    {
        delete calendarDialog;
        calendarDialog = 0;
    }
}

void RazorClock::showConfigureDialog()
{
    RazorClockConfiguration *confWindow = this->findChild<RazorClockConfiguration*>("ClockConfigurationWindow");

    if (!confWindow)
    {
        confWindow = new RazorClockConfiguration(settings(), this);
    }

    confWindow->show();
    confWindow->raise();
    confWindow->activateWindow();
}



bool ClockWidget::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        setToolTip(QDateTime::currentDateTime().toString(Qt::DefaultLocaleLongDate));
    }

    return QWidget::event(event);
}

bool ClockLabel::event(QEvent *event)
{
    if (event->type() == QEvent::FontChange)
    {
        emit fontChanged();
    }

    return QLabel::event(event);
}

#endif
