/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2013 Razor team
 * Authors:
 *   Christopher "VdoP" Regali
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *   Maciej Płaza <plaza.maciej@gmail.com>
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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


#include "razorclock.h"
#include "calendar_utils.h"

#include <QtGui/QCalendarWidget>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QMouseEvent>

#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtCore/QPoint>
#include <QtCore/QRect>

#include <QtCore/QDebug>

/**
 * @file razorclock.cpp
 * @brief implements Razorclock and Razorclockgui
 * @author Christopher "VdoP" Regali
 * @author Kuzma Shapran
 */
Q_EXPORT_PLUGIN2(clock, RazorClockPluginLibrary)

/**
 * @brief constructor
 */
RazorClock::RazorClock(const IRazorPanelPluginStartupInfo &startupInfo):
    QObject(),
    IRazorPanelPlugin(startupInfo),
    mCalendarDialog(0),
    mAutoRotate(true)
{
    mMainWidget = new QWidget();
    mRotatedWidget = new RotatedWidget(*(new QWidget()), mMainWidget);
    mContent = mRotatedWidget->content();
    mTimeLabel = new QLabel(mContent);
    mDateLabel = new QLabel(mContent);

    QVBoxLayout *borderLayout = new QVBoxLayout(mMainWidget);
    borderLayout->setContentsMargins(0, 0, 0, 0);
    borderLayout->setSpacing(0);
    borderLayout->addWidget(mRotatedWidget, 0, Qt::AlignCenter);

    mTimeLabel->setObjectName("TimeLabel");
    mDateLabel->setObjectName("DateLabel");

    mTimeLabel->setAlignment(Qt::AlignCenter);
    mDateLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout *contentLayout = new QVBoxLayout(mContent);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(1);
    contentLayout->addWidget(mTimeLabel, 0, Qt::AlignCenter);
    contentLayout->addWidget(mDateLabel, 0, Qt::AlignCenter);
    mContent->setLayout(contentLayout);

    mTimeLabel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    mDateLabel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    mContent->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    mRotatedWidget->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    mMainWidget->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));


    mClockTimer = new QTimer(this);
    connect (mClockTimer, SIGNAL(timeout()), SLOT(updateTime()));

    mClockFormat = "hh:mm";

    mFirstDayOfWeek = firstDayOfWeek();

    mMainWidget->installEventFilter(this);
    settingsChanged();
}

/**
 * @brief destructor
 */
RazorClock::~RazorClock()
{
    delete mMainWidget;
}

QDateTime RazorClock::currentDateTime()
{
#if QT_VERSION < 0x040700
    return QDateTime(mUseUTC ? QDateTime::currentDateTime().toUTC() : QDateTime::currentDateTime());
#else
    return QDateTime(mUseUTC ? QDateTime::currentDateTimeUtc() : QDateTime::currentDateTime());
#endif
}

/**
 * @brief updates the time
 * Color and font settings can be configured in Qt CSS
 */
void RazorClock::updateTime()
{
    QDateTime now = currentDateTime();

    if (now.time().msec() > 500)
        restartTimer(now);

    showTime(now);
}

void RazorClock::showTime(const QDateTime &now)
{
    if (mDateOnNewLine)
    {
        mTimeLabel->setText(QLocale::system().toString(now, mTimeFormat));
        mDateLabel->setText(QLocale::system().toString(now, mDateFormat));
    }
    else
    {
        mTimeLabel->setText(QLocale::system().toString(now, mClockFormat));
    }

    mRotatedWidget->adjustContentSize();

    mRotatedWidget->update();
}

void RazorClock::restartTimer(const QDateTime &now)
{
    if (mClockTimer->isActive())
        mClockTimer->stop();
    int updateInterval = mClockTimer->interval();
    int delay = static_cast<int>((updateInterval + 100 /* ms after time change */ - ((now.time().msec() + now.time().second() * 1000) % updateInterval)) % updateInterval);
    QTimer::singleShot(delay, this, SLOT(updateTime()));
    QTimer::singleShot(delay, mClockTimer, SLOT(start()));
}

void RazorClock::settingsChanged()
{
    mTimeFormat = settings()->value("timeFormat", QLocale::system().timeFormat(QLocale::ShortFormat).toUpper().contains("AP") ? "h:mm AP" : "HH:mm").toString();

    mUseUTC = settings()->value("UTC", false).toBool();
    if (mUseUTC)
        mTimeFormat += "' Z'";

    mDateFormat = settings()->value("dateFormat", Qt::SystemLocaleShortDate).toString();

    bool dateBeforeTime = (settings()->value("showDate", "no").toString().toLower() == "before");
    bool dateAfterTime = (settings()->value("showDate", "no").toString().toLower() == "after");
    mDateOnNewLine = (settings()->value("showDate", "no").toString().toLower() == "below");

    bool autoRotate = settings()->value("autoRotate", true).toBool();
    if (autoRotate != mAutoRotate)
    {
        mAutoRotate = autoRotate;
        realign();
    }


    if (dateBeforeTime)
        mClockFormat = QString("%1 %2").arg(mDateFormat).arg(mTimeFormat);
    else if (dateAfterTime)
        mClockFormat = QString("%1 %2").arg(mTimeFormat).arg(mDateFormat);
    else
        mClockFormat = mTimeFormat;

    mDateLabel->setVisible(mDateOnNewLine);

    updateMinWidth();

    // mDateFormat usually does not contain time portion, but since it's possible to use custom date format - it has to be supported. [Kuzma Shapran]
    int updateInterval = QString(mTimeFormat + " " + mDateFormat).replace(QRegExp("'[^']*'"),"").contains("s") ? 1000 : 60000;

    QDateTime now = currentDateTime();

    showTime(now);

    if (mClockTimer->interval() != updateInterval)
    {
        mClockTimer->setInterval(updateInterval);

        restartTimer(now);
    }
}

void RazorClock::realign()
{
    if (mAutoRotate)
        switch (panel()->position())
        {
        case IRazorPanel::PositionTop:
        case IRazorPanel::PositionBottom:
            mRotatedWidget->setOrigin(Qt::TopLeftCorner);
            break;

        case IRazorPanel::PositionLeft:
            mRotatedWidget->setOrigin(Qt::BottomLeftCorner);
            break;

        case IRazorPanel::PositionRight:
            mRotatedWidget->setOrigin(Qt::TopRightCorner);
            break;
        }
    else
        mRotatedWidget->setOrigin(Qt::TopLeftCorner);
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
    QFontMetrics timeLabelMetrics(mTimeLabel->font());
    QFontMetrics dateLabelMetrics(mDateLabel->font());
    QDate maxDate = getMaxDate(mDateOnNewLine ? dateLabelMetrics : timeLabelMetrics, mDateFormat);
    QTime maxTime = getMaxTime(timeLabelMetrics, mTimeFormat);
    QDateTime dt(maxDate, maxTime);

    //qDebug() << "T:" << metrics.boundingRect(dt.toString(mTimeFormat)).width();
    //qDebug() << "C:" << metrics.boundingRect(QTime::currentTime().toString(mTimeFormat)).width() << QTime::currentTime().toString(mTimeFormat);
    //qDebug() << "D:" << metrics.boundingRect(dt.toString(mDateFormat)).width();

    int width;
    int height;
    if (mDateOnNewLine)
    {
        QRect rect1(timeLabelMetrics.boundingRect(dt.toString(mTimeFormat)));
        mTimeLabel->setMinimumSize(rect1.size());
        QRect rect2(dateLabelMetrics.boundingRect(dt.toString(mDateFormat)));
        mDateLabel->setMinimumSize(rect2.size());
        width = qMax(rect1.width(), rect2.width());
        height = rect1.height() + rect2.height();
//        qDebug() << "RazorClock Recalc size" << width << height << dt.toString(mTimeFormat) << dt.toString(mDateFormat);
    }
    else
    {
        QRect rect(timeLabelMetrics.boundingRect(dt.toString(mClockFormat)));
        mTimeLabel->setMinimumSize(rect.size());
        mDateLabel->setMinimumSize(0, 0);
        width = rect.width();
        height = rect.height();
//        qDebug() << "RazorClock Recalc size" << width << height << dt.toString(mClockFormat);
    }


    mContent->setMinimumSize(width, height);
}

void RazorClock::activated(ActivationReason reason)
{
    if (reason != IRazorPanelPlugin::Trigger)
        return;

    if (!mCalendarDialog)
    {
        mCalendarDialog = new QDialog(mContent);
        //mCalendarDialog->setAttribute(Qt::WA_DeleteOnClose, true);
        mCalendarDialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | Qt::X11BypassWindowManagerHint);
        mCalendarDialog->setLayout(new QHBoxLayout(mCalendarDialog));
        mCalendarDialog->layout()->setMargin(1);

        QCalendarWidget* cal = new QCalendarWidget(mCalendarDialog);
        cal->setFirstDayOfWeek(mFirstDayOfWeek);
        mCalendarDialog->layout()->addWidget(cal);
        mCalendarDialog->adjustSize();
        QRect pos = calculatePopupWindowPos(mCalendarDialog->size());
        mCalendarDialog->move(pos.topLeft());
        mCalendarDialog->show();
    }
    else
    {
        delete mCalendarDialog;
        mCalendarDialog = 0;
    }
}

QDialog * RazorClock::configureDialog()
{
     return new RazorClockConfiguration(*settings());
}

bool RazorClock::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == mMainWidget)
    {
        if (event->type() == QEvent::ToolTip)
        {
            mMainWidget->setToolTip(QDateTime::currentDateTime().toString(Qt::DefaultLocaleLongDate));
        }

        return false;
    }

    return false;
}
