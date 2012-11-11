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

#include <unicode/unistr.h>
#include <unicode/locid.h>
#include <unicode/calendar.h>
#include <unicode/datefmt.h>
#include <unicode/smpdtfmt.h>
#include <unicode/timezone.h>
#include <unicode/uclean.h>

#include <QtGui/QLabel>
#include <QtCore/QLocale>
#include <QtCore/QTimer>
#include <QtCore/QScopedArrayPointer>
#include <QtCore/QDebug>

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

static icu::TimeZone* createZone(const icu::UnicodeString &id)
{
    icu::UnicodeString str;
    icu::TimeZone* zone = icu::TimeZone::createTimeZone(id);
    if (zone->getID(str) != id)
    {
        delete zone;
        zone = NULL;
        qDebug() << "Error: icu::TimeZone::createTimeZone(" << ICU_to_Qt(id) << ") returned zone with ID " << ICU_to_Qt(str);
    }
    return zone;
}


RazorWorldClock::RazorWorldClock(const RazorPanelPluginStartInfo *startInfo, QWidget *parent):
    RazorPanelPlugin(startInfo, parent),
    mContent(new QLabel(this)),
    mTimer(new QTimer(this)),
    mSynchroTimer(new QTimer(this)),
    mLastSynchro(0),
    mFormatType(FORMAT__INVALID),
    mFormat(NULL)
{
    setObjectName("WorldClock");

    mContent->setAlignment(Qt::AlignCenter);

    addWidget(mContent);


    UErrorCode status = U_ZERO_ERROR;
    mLocale = new icu::Locale(QLocale::languageToString(locale().language()).toAscii());
    mCalendar = icu::Calendar::createInstance(status);


    settingsChanged();

    mActiveTimeZone = mDefaultTimeZone;

    updateFormat();
    updateTimezone();

    connect(mTimer, SIGNAL(timeout()), SLOT(timeout()));
    connect(mSynchroTimer, SIGNAL(timeout()), SLOT(synchroTimeout()));

    mTimer->setInterval(1000);
    mSynchroTimer->setInterval(10);
    mSynchroTimer->start();
}

RazorWorldClock::~RazorWorldClock()
{
    u_cleanup();
}

void RazorWorldClock::synchroTimeout(void)
{
    time_t synchro = time(NULL);
    if ((mLastSynchro != 0) && (mLastSynchro != synchro))
    {
        mSynchroTimer->stop();
        mTimer->start();
    }
    mLastSynchro = synchro;
}

void RazorWorldClock::timeout(void)
{
    UErrorCode status = U_ZERO_ERROR;

    UnicodeString str;
    mFormat->format(Calendar::getNow(), str, status);
    mContent->setText(ICU_to_Qt(str));
}

void RazorWorldClock::updateFormat(void)
{
    if (mFormat)
        delete mFormat;

    UErrorCode status = U_ZERO_ERROR;

    switch (mFormatType)
    {
    case FORMAT_CUSTOM:
        mFormat = new icu::SimpleDateFormat(Qt_to_ICU(mCustomFormat), *mLocale, status);
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

    mFormat->setCalendar(*mCalendar);
}

void RazorWorldClock::updateTimezone(void)
{
    mCalendar->adoptTimeZone(createZone(mActiveTimeZone.toAscii().data()));
    mFormat->setCalendar(*mCalendar);
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

    mCustomFormat = _settings.value("customFormat", QString()).toString();

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
