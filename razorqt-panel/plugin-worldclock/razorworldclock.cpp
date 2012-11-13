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

#include <time.h>


EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorWorldClock)


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


RazorWorldClock::RazorWorldClock(const RazorPanelPluginStartInfo *startInfo, QWidget *parent):
    RazorPanelPlugin(startInfo, parent),
    mContent(new ActiveLabel(this)),
    mPopupCalendar(NULL),
    mTimer(new QTimer(this)),
    mFormatType(FORMAT__INVALID),
    mCalendar(NULL),
    mFormat(NULL)
{
    setObjectName("WorldClock");

    mContent->setAlignment(Qt::AlignCenter);

    addWidget(mContent);


    mLocale = new icu::Locale();
    mDefaultLanguage = QString(mLocale->getLanguage());


    settingsChanged();

    connect(mTimer, SIGNAL(timeout()), SLOT(timeout()));

    connect(mContent, SIGNAL(wheelScrolled(int)), SLOT(wheelScrolled(int)));
    connect(mContent, SIGNAL(leftMouseButtonClicked()), SLOT(leftMouseButtonClicked()));
    connect(mContent, SIGNAL(middleMouseButtonClicked()), SLOT(middleMouseButtonClicked()));

    mTimer->setInterval(100); // faster than 1 sec to support milliseconds at least partially
    mTimer->start();
}

RazorWorldClock::~RazorWorldClock()
{
    if (mFormat)
        delete mFormat;
    if (mCalendar)
        delete mCalendar;
    if (mLocale)
        delete mLocale;
    u_cleanup();
}

void RazorWorldClock::timeout(void)
{
    if (mFormat)
    {
        UErrorCode status = U_ZERO_ERROR;
        UnicodeString str;
        mFormat->format(Calendar::getNow(), str, status);
        if (U_FAILURE(status))
            qDebug() << "timeout: status = " << status;

        if (str != mLastShownText)
        {
            mContent->setText(ICU_to_Qt(str));
            mLastShownText = str;
        }
    }
}

void RazorWorldClock::updateFormat(void)
{
    if (mFormat)
        delete mFormat;


    switch (mFormatType)
    {
    case FORMAT_CUSTOM:
    {
        UErrorCode status = U_ZERO_ERROR;
        mFormat = new icu::SimpleDateFormat(Qt_to_ICU(mCustomFormat), *mLocale, status);
        if (U_FAILURE(status))
            qDebug() << "updateFormat: status = " << status;
    }
        break;

    case FORMAT_FULL:
        mFormat = icu::DateFormat::createDateTimeInstance(DateFormat::kFull, DateFormat::kFull, *mLocale);
        break;

    case FORMAT_LONG:
        mFormat = icu::DateFormat::createDateTimeInstance(DateFormat::kLong, DateFormat::kLong, *mLocale);
        break;

    case FORMAT_MEDIUM:
        mFormat = icu::DateFormat::createDateTimeInstance(DateFormat::kMedium, DateFormat::kMedium, *mLocale);
        break;

    case FORMAT_SHORT:
        mFormat = icu::DateFormat::createDateTimeInstance(DateFormat::kShort, DateFormat::kShort, *mLocale);
        break;

    default:;
    }

    if (mCalendar)
        mFormat->setCalendar(*mCalendar);
}

void RazorWorldClock::updateTimezone(void)
{
    if (mFormat)
    {
        if (mCalendar)
            delete mCalendar;

        UErrorCode status = U_ZERO_ERROR;
        char region[3];
        icu::TimeZone::getRegion(mActiveTimeZone.toAscii().data(), region, sizeof(region) / sizeof(char), status);
        if (U_FAILURE(status))
            qDebug() << "updateTimezone:getRegion: status = " << status;

        if (mLocale)
            delete mLocale;
        mLocale = new icu::Locale(mDefaultLanguage.toAscii().data(), region);

        icu::UnicodeString timeZoneName = Qt_to_ICU(mActiveTimeZone);
        icu::TimeZone* timeZone = icu::TimeZone::createTimeZone(timeZoneName);
        icu::UnicodeString control;
        if (timeZone->getID(control) != timeZoneName)
            qDebug() << "Error: icu::TimeZone::createTimeZone(" << mActiveTimeZone << ") returned zone with ID " << ICU_to_Qt(control);

        status = U_ZERO_ERROR;
        mCalendar = icu::Calendar::createInstance(timeZone, *mLocale, status);
        if (U_FAILURE(status))
            qDebug() << "updateTimezone:Calendar: status = " << status;
        mFormat->setCalendar(*mCalendar);
    }
}

void RazorWorldClock::settingsChanged()
{
    QSettings &_settings = settings();

    FormatType oldFormatType = mFormatType;
    QString oldCustomFormat = mCustomFormat;

    mTimeZones.clear();

    int size = _settings.beginReadArray("timeZones");
    for (int i = 0; i < size; ++i)
    {
        _settings.setArrayIndex(i);
        mTimeZones.append(_settings.value("timeZone", QString()).toString());
    }
    _settings.endArray();

    mDefaultTimeZone = _settings.value("defaultTimeZone", QString()).toString();
    if (mDefaultTimeZone.isEmpty() && !mTimeZones.isEmpty())
        mDefaultTimeZone = mTimeZones[0];
    if (mDefaultTimeZone.isEmpty())
    {
        icu::TimeZone *timeZone = icu::TimeZone::createDefault();
        icu::UnicodeString timeZoneName;
        timeZone->getID(timeZoneName);
        mDefaultTimeZone = ICU_to_Qt(timeZoneName);

        delete timeZone;
    }
    mActiveTimeZone = mDefaultTimeZone;

    mCustomFormat = _settings.value("customFormat", QString("'<b>'HH:mm:ss'</b><br/><font size=\"-2\">'eee, d MMM yyyy'<br/>'VVVV'</font>'")).toString();

    QString formatType = _settings.value("formatType", QString()).toString();
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

void RazorWorldClock::showConfigureDialog()
{
    RazorWorldClockConfiguration *confWindow = this->findChild<RazorWorldClockConfiguration*>("WorldClockConfigurationWindow");

    if (!confWindow)
        confWindow = new RazorWorldClockConfiguration(settings(), this);

    confWindow->show();
    confWindow->raise();
    confWindow->activateWindow();
}

void RazorWorldClock::wheelScrolled(int delta)
{
    if (mTimeZones.count() > 1)
    {
        mActiveTimeZone = mTimeZones[(mTimeZones.indexOf(mActiveTimeZone) + ((delta > 0) ? -1 : 1) + mTimeZones.size()) % mTimeZones.size()];

        updateTimezone();
        timeout(); // instantly!
    }
}

void RazorWorldClock::leftMouseButtonClicked(void)
{
    if (!mPopupCalendar)
    {
        mPopupCalendar = new QDialog(this);
        mPopupCalendar->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        mPopupCalendar->setLayout(new QHBoxLayout(mPopupCalendar));
        mPopupCalendar->layout()->setMargin(0);
        QCalendarWidget *calendarWidget = new QCalendarWidget(mPopupCalendar);
        mPopupCalendar->layout()->addWidget(calendarWidget);
        mPopupCalendar->adjustSize();

        UErrorCode status = U_ZERO_ERROR;
        UCalendarDaysOfWeek first = mCalendar->getFirstDayOfWeek(status);
        if (U_FAILURE(status))
            qDebug() << "leftMouseButtonClicked:getFirstDayOfWeek: status = " << status;
        calendarWidget->setFirstDayOfWeek(static_cast<Qt::DayOfWeek>(((static_cast<int>(first) + 5) % 7) + 1));

        status = U_ZERO_ERROR;
        calendarWidget->setSelectedDate(QDate(mCalendar->get(UCAL_YEAR, status), mCalendar->get(UCAL_MONTH, status) - UCAL_JANUARY + 1, mCalendar->get(UCAL_DATE, status)));
        if (U_FAILURE(status))
            qDebug() << "leftMouseButtonClicked:get: status = " << status;

        QSize calSize = mPopupCalendar->size();
        QPoint center = panel()->mapToGlobal((this->geometry().topLeft() + this->geometry().bottomRight()) / 2);
        QRect avail = QDesktopWidget().availableGeometry(center);
        if (center.x() < avail.left())
            center.setX(avail.left());
        else if (center.x() > avail.right() - calSize.width())
            center.setX(avail.right() - calSize.width());
        if (center.y() < avail.top())
            center.setY(avail.top());
        else if (center.y() > avail.bottom() - calSize.height())
            center.setY(avail.bottom() - calSize.height());

        mPopupCalendar->move(center);
        mPopupCalendar->show();
    }
    else
    {
        mPopupCalendar->deleteLater();
        mPopupCalendar = NULL;
    }
}

void RazorWorldClock::middleMouseButtonClicked(void)
{
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
