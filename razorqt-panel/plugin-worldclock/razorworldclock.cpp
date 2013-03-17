/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
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

#include "razorworldclock.h"

#include <unicode/locid.h>
#include <unicode/calendar.h>
#include <unicode/datefmt.h>
#include <unicode/smpdtfmt.h>
#include <unicode/timezone.h>
#include <unicode/uclean.h>

#include <QtCore/QLocale>
#include <QtCore/QTimer>
#include <QtCore/QScopedArrayPointer>
#include <QtCore/QDate>
#include <QtCore/QDebug>
#include <QtGui/QWheelEvent>
#include <QtGui/QCalendarWidget>
#include <QtGui/QDesktopWidget>
#include <QHBoxLayout>

#include <time.h>


Q_EXPORT_PLUGIN2(panelworldclock, RazorWorldClockLibrary)


static QString ICU_to_Qt(const icu::UnicodeString &string)
{
    UErrorCode status = U_ZERO_ERROR;

    int32_t len32 = string.countChar32();
    QScopedArrayPointer<UChar32> uch32(new UChar32[len32]);
    string.toUTF32(uch32.data(), len32, status);
    QScopedArrayPointer<uint> ui32(new uint[len32]);
    for (int32_t i = 0; i < len32; ++i)
        ui32[i] = uch32[i];
    return QString::fromUcs4(ui32.data(), len32);
}

static icu::UnicodeString Qt_to_ICU(const QString &string)
{
    QVector<uint> ucs = string.toUcs4();
    int len = ucs.size();
    QScopedArrayPointer<UChar32> uch32(new UChar32[len]);
    for (int i = 0; i < len; ++i)
        uch32[i] = ucs[i];
    return icu::UnicodeString::fromUTF32(uch32.data(), len);
}


RazorWorldClock::RazorWorldClock(const IRazorPanelPluginStartupInfo &startupInfo):
    QObject(),
    IRazorPanelPlugin(startupInfo),
    mContent(new ActiveLabel()),
    mPopup(NULL),
    mTimer(new QTimer(this)),
    mFormatType(FORMAT__INVALID),
    mCalendar(NULL),
    mFormat(NULL)
{
    mContent->setObjectName("WorldClockContent");

    mContent->setAlignment(Qt::AlignCenter);

    mLocale = new icu::Locale();
    mDefaultLanguage = QString(mLocale->getLanguage());

    settingsChanged();

    connect(mTimer, SIGNAL(timeout()), SLOT(timeout()));

    connect(mContent, SIGNAL(wheelScrolled(int)), SLOT(wheelScrolled(int)));
    connect(mContent, SIGNAL(leftMouseButtonClicked()), SLOT(leftMouseButtonClicked()));
    connect(mContent, SIGNAL(middleMouseButtonClicked()), SLOT(middleMouseButtonClicked()));
}

RazorWorldClock::~RazorWorldClock()
{
    delete mContent;

    if (mFormat)
        delete mFormat;
    if (mCalendar)
        delete mCalendar;
    if (mLocale)
        delete mLocale;
    u_cleanup();
}

QWidget *RazorWorldClock::widget()
{
    return mContent;
}


void RazorWorldClock::timeout()
{
    if (mFormat)
    {
        UErrorCode status = U_ZERO_ERROR;
        UnicodeString str;

        UDate now = Calendar::getNow();

        if ((mTimer->interval() >= 1000) && ((static_cast<long long>(now) % 1000) > 200))
            restartTimer(mTimer->interval());

        mFormat->format(now, str, status);
        if (U_FAILURE(status))
            qDebug() << "timeout: Calendar::getNow() status = " << status;

        if (str != mLastShownText)
        {
            mContent->setText(ICU_to_Qt(str));
            mLastShownText = str;
        }
    }
}

void RazorWorldClock::updateFormat()
{
    if (mFormat)
        delete mFormat;

    int timerInterval = 0;

    switch (mFormatType)
    {
    case FORMAT_CUSTOM:
    {
        UErrorCode status = U_ZERO_ERROR;
        mFormat = new icu::SimpleDateFormat(Qt_to_ICU(mCustomFormat), *mLocale, status);
        if (U_FAILURE(status))
            qDebug() << "updateFormat: SimpleDateFormat() = " << status;

        QString format = mCustomFormat;
        format.replace(QRegExp(QLatin1String("'[^']*'")), QString());
        if (format.contains(QString("SSS")))
            timerInterval = 1;
        else if (format.contains(QString("SS")))
            timerInterval = 10;
        else if (format.contains(QString("S")))
            timerInterval = 100;
        else if (format.contains(QString("s")))
            timerInterval = 1000;
        else
            timerInterval = 60000;
    }
        break;

    case FORMAT_FULL:
        mFormat = icu::DateFormat::createDateTimeInstance(DateFormat::kFull, DateFormat::kFull, *mLocale);
        timerInterval = 1000;
        break;

    case FORMAT_LONG:
        mFormat = icu::DateFormat::createDateTimeInstance(DateFormat::kLong, DateFormat::kLong, *mLocale);
        timerInterval = 1000;
        break;

    case FORMAT_MEDIUM:
        mFormat = icu::DateFormat::createDateTimeInstance(DateFormat::kMedium, DateFormat::kMedium, *mLocale);
        timerInterval = 1000;
        break;

    case FORMAT_SHORT:
        mFormat = icu::DateFormat::createDateTimeInstance(DateFormat::kShort, DateFormat::kShort, *mLocale);
        timerInterval = 60000;
        break;

    default:;
    }

    restartTimer(timerInterval);

    if (mCalendar)
        mFormat->setCalendar(*mCalendar);
}

void RazorWorldClock::restartTimer(int timerInterval)
{
    mTimer->stop();
    mTimer->setInterval(timerInterval);

    if (timerInterval < 1000)
        mTimer->start();
    else
    {
        int delay = static_cast<int>((timerInterval + 100 - (static_cast<long long>(Calendar::getNow()) % timerInterval)) % timerInterval);
        QTimer::singleShot(delay, this, SLOT(timeout()));
        QTimer::singleShot(delay, mTimer, SLOT(start()));
    }
}

void RazorWorldClock::updateTimezone()
{
    if (mFormat)
    {
        if (mCalendar)
            delete mCalendar;

        UErrorCode status = U_ZERO_ERROR;

#if ICU_VERSION >= 48
        char region[3];
        icu::TimeZone::getRegion(mActiveTimeZone.toAscii().data(), region, sizeof(region) / sizeof(char), status);
        if (U_FAILURE(status))
            qDebug() << "updateTimezone: TimeZone::getRegion() status = " << status;

        if (mLocale)
            delete mLocale;
        mLocale = new icu::Locale(mDefaultLanguage.toAscii().data(), region);
#endif

        icu::UnicodeString timeZoneName = Qt_to_ICU(mActiveTimeZone);
        icu::TimeZone* timeZone = icu::TimeZone::createTimeZone(timeZoneName);
        icu::UnicodeString control;
        if (timeZone->getID(control) != timeZoneName)
            qDebug() << "updateTimezone: TimeZone::createTimeZone(" << mActiveTimeZone << ") returned zone with ID " << ICU_to_Qt(control);

        status = U_ZERO_ERROR;
        mCalendar = icu::Calendar::createInstance(timeZone, *mLocale, status);
        if (U_FAILURE(status))
            qDebug() << "updateTimezone: Calendar::createInstance() status = " << status;
        mFormat->setCalendar(*mCalendar);
    }

    timeout(); // instantly!
}

void RazorWorldClock::settingsChanged()
{
    QSettings *_settings = settings();

    FormatType oldFormatType = mFormatType;
    QString oldCustomFormat = mCustomFormat;

    mTimeZones.clear();

    int size = _settings->beginReadArray("timeZones");
    for (int i = 0; i < size; ++i)
    {
        _settings->setArrayIndex(i);
        mTimeZones.append(_settings->value("timeZone", QString()).toString());
    }
    _settings->endArray();
    if (mTimeZones.isEmpty())
    {
        icu::TimeZone *timeZone = icu::TimeZone::createDefault();
        icu::UnicodeString timeZoneName;
        timeZone->getID(timeZoneName);
        mTimeZones.append(ICU_to_Qt(timeZoneName));

        delete timeZone;
    }

    mDefaultTimeZone = _settings->value("defaultTimeZone", QString()).toString();
    if (mDefaultTimeZone.isEmpty())
        mDefaultTimeZone = mTimeZones[0];
    mActiveTimeZone = mDefaultTimeZone;

    mCustomFormat = _settings->value("customFormat", QString("'<b>'HH:mm:ss'</b><br/><font size=\"-2\">'eee, d MMM yyyy'<br/>'VVVV'</font>'")).toString();

    QString formatType = _settings->value("formatType", QString()).toString();
    if (formatType == "custom")
        mFormatType = FORMAT_CUSTOM;
    else if (formatType == "full")
        mFormatType = FORMAT_FULL;
    else if (formatType == "long")
        mFormatType = FORMAT_LONG;
    else if (formatType == "medium")
        mFormatType = FORMAT_MEDIUM;
    else
        mFormatType = FORMAT_SHORT;

    if ((oldFormatType != mFormatType) || (oldCustomFormat != mCustomFormat))
        updateFormat();

    updateTimezone();
}

QDialog *RazorWorldClock::configureDialog()
{
    return new RazorWorldClockConfiguration(settings());
}

void RazorWorldClock::wheelScrolled(int delta)
{
    if (mTimeZones.count() > 1)
    {
        mActiveTimeZone = mTimeZones[(mTimeZones.indexOf(mActiveTimeZone) + ((delta > 0) ? -1 : 1) + mTimeZones.size()) % mTimeZones.size()];

        updateTimezone();
    }
}

void RazorWorldClock::popupDialog(bool withCalendar)
{
    if (!mPopup)
    {
        UErrorCode status = U_ZERO_ERROR;

        mPopup = new QDialog(mContent);
        mPopup->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        mPopup->setLayout(new QHBoxLayout(mPopup));

        if (withCalendar)
        {
            mPopup->layout()->setContentsMargins(0, 0, 0, 0);
            QCalendarWidget *calendarWidget = new QCalendarWidget(mPopup);
            mPopup->layout()->addWidget(calendarWidget);

            status = U_ZERO_ERROR;
            UCalendarDaysOfWeek first = mCalendar->getFirstDayOfWeek(status);
            if (U_FAILURE(status))
                qDebug() << "popupDialog:calendar: Calendar::getFirstDayOfWeek() status = " << status;
            calendarWidget->setFirstDayOfWeek(static_cast<Qt::DayOfWeek>(((static_cast<int>(first) + 5) % 7) + 1));

            status = U_ZERO_ERROR;
            calendarWidget->setSelectedDate(QDate(mCalendar->get(UCAL_YEAR, status), mCalendar->get(UCAL_MONTH, status) - UCAL_JANUARY + 1, mCalendar->get(UCAL_DATE, status)));
            if (U_FAILURE(status))
                qDebug() << "popupDialog:calendar: Calendar:get() status = " << status;
        }
        else
        {
            QLabel *content = new QLabel(mPopup);
            content->setObjectName("WorldClockPopup");
            mPopup->layout()->addWidget(content);

            QStringList allTimeZones;

            foreach (QString qTimeZoneName, mTimeZones)
            {
                icu::UnicodeString timeZoneName = Qt_to_ICU(qTimeZoneName);
                icu::TimeZone* timeZone = icu::TimeZone::createTimeZone(timeZoneName);
                icu::UnicodeString control;
                if (timeZone->getID(control) != timeZoneName)
                    qDebug() << "popupDialog:time: TimeZone::createTimeZone(" << mActiveTimeZone << ") returned zone with ID " << ICU_to_Qt(control);

                status = U_ZERO_ERROR;
                icu::Calendar *calendar = icu::Calendar::createInstance(timeZone, *mLocale, status);
                if (U_FAILURE(status))
                    qDebug() << "popupDialog:time: Calendar::createInstance() status = " << status;
                mFormat->setCalendar(*calendar);

                status = U_ZERO_ERROR;
                UnicodeString str;
                mFormat->format(Calendar::getNow(), str, status);
                if (U_FAILURE(status))
                    qDebug() << "popupDialog:time: DateFormat::format() status = " << status;

                delete calendar;

                allTimeZones.append(ICU_to_Qt(str));
            }

            content->setText(allTimeZones.join("<hr/>"));

            //restore current
            mFormat->setCalendar(*mCalendar);

            content->setAlignment(mContent->alignment());
        }
        mPopup->adjustSize();

        QRect rect = calculatePopupWindowPos(mPopup->size());
        mPopup->setGeometry(rect);
        mPopup->show();
    }
    else
    {
        mPopup->deleteLater();
        mPopup = NULL;
    }
}

void RazorWorldClock::leftMouseButtonClicked()
{
    popupDialog(true);
}

void RazorWorldClock::middleMouseButtonClicked()
{
    popupDialog(false);
}

ActiveLabel::ActiveLabel(QWidget * parent) :
    QLabel(parent)
{
}

void ActiveLabel::wheelEvent(QWheelEvent *event)
{
    emit wheelScrolled(event->delta());

    QLabel::wheelEvent(event);
}

void ActiveLabel::mouseReleaseEvent(QMouseEvent* event)
{
    switch (event->button())
    {
    case Qt::LeftButton:
        emit leftMouseButtonClicked();
        break;

    case Qt::MidButton:
        emit middleMouseButtonClicked();
        break;

    default:;
    }

    QLabel::mouseReleaseEvent(event);
}
