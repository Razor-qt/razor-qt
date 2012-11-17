/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
 *   Luís Pereira <luis.artur.pereira@gmail.com>
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


#include "calendar_utils.h"
#include "config.h"

#include <QtCore/QtGlobal>

#if QT_VERSION < 0x040800

#ifdef CAN_USE_BOOST

#include <boost/locale/date_time.hpp>

Qt::DayOfWeek firstDayOfWeek(void)
{
    return (boost::locale::calendar().first_day_of_week() + 6) % 7;
}

#else // use C

#ifdef HAVE__NL_TIME_FIRST_WEEKDAY
#include <langinfo.h>
#include <QtCore/QDebug>
#endif

Qt::DayOfWeek firstDayOfWeek(void)
{
    #ifdef HAVE__NL_TIME_FIRST_WEEKDAY
    int firstWeekDay;
    int weekFirstDay = 0;
    int weekStart;
    char *locale;

    long weekFirstDayLong;

    Q_UNUSED(locale);
    locale = setlocale(LC_TIME, "");

    // firstWeekDay: Specifies the offset of the first day-of-week in the day
    // list.

    // weekFirstDay: Some date that corresponds to the beginning of a week.
    // Specifies the base of the day list. It is (in glibc) either
    // 19971130 (Sunday) or 19971201 (Monday)

    firstWeekDay = nl_langinfo(_NL_TIME_FIRST_WEEKDAY)[0];

    weekFirstDayLong = (long) nl_langinfo(_NL_TIME_WEEK_1STDAY);
    if (weekFirstDayLong == 19971130L)  // Sunday
    {
        weekFirstDay = 0;
    }
    else if (weekFirstDayLong == 19971201L) // Monday
    {
        weekFirstDay = 1;
    }
    else
    {
        qDebug() << Q_FUNC_INFO <<
            "nl_langinfo(_NL_TIME_WEEK_1STDAY) returned an unknown value.";
    }

    weekStart = (weekFirstDay + firstWeekDay - 1) % 7;
    if (weekStart == 0)
    {
        return Qt::Sunday;
    }
    else
    {
        return static_cast<Qt::DayOfWeek>(weekStart);
    }
    #else
    return Qt::Sunday;
    #endif // HAVE__NL_TIME_FIRST_WEEKDAY
}

#endif

#else // use Qt >= 4.8

Qt::DayOfWeek firstDayOfWeek(void)
{
    return QLocale::system().firstDayOfWeek();
}

#endif
