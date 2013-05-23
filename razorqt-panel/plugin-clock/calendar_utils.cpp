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


#if QT_VERSION >= 0x040800

Qt::DayOfWeek firstDayOfWeek()
{
    return QLocale::system().firstDayOfWeek();
}

#elif defined CAN_USE_BOOST

#include <boost/locale/date_time.hpp>

Qt::DayOfWeek firstDayOfWeek()
{
    return (boost::locale::calendar().first_day_of_week() + 6) % 7;
}

#elif defined HAVE__NL_TIME_FIRST_WEEKDAY

#include <langinfo.h>
#include <locale.h>
#include <QtCore/QDebug>

Qt::DayOfWeek firstDayOfWeek()
{
    static char *locale = NULL;
    if (!locale)
        locale = setlocale(LC_TIME, "");

    // firstWeekDay: Specifies the offset of the first day-of-week in the day
    // list.

    // weekFirstDay: Some date that corresponds to the beginning of a week.
    // Specifies the base of the day list. It is (in glibc) either
    // 19971130 (Sunday) or 19971201 (Monday)

    int firstWeekDay = nl_langinfo(_NL_TIME_FIRST_WEEKDAY)[0];

    int weekFirstDay = 0;
    long weekFirstDayLong = reinterpret_cast<long>(nl_langinfo(_NL_TIME_WEEK_1STDAY));
    switch (weekFirstDayLong)
    {
    case 19971130L: // Sunday
        weekFirstDay = 0;
        break;

    case 19971201L: // Monday
        weekFirstDay = 1;
        break;

    default:
        qDebug() << Q_FUNC_INFO <<
            "nl_langinfo(_NL_TIME_WEEK_1STDAY) returned an unknown value.";
    }

    // conversion between C constants (Sunday = 1 .. Saturday = 7) and Qt (Monday = 1 .. Sunday = 7)
    int weekStart = ((weekFirstDay + firstWeekDay + 5) % 7) + 1;

    return static_cast<Qt::DayOfWeek>(weekStart);
}

#else

Qt::DayOfWeek firstDayOfWeek()
{
    return Qt::Sunday;
}

#endif // QT_VERSION >= 0x040800
