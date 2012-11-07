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
    int first_weekday, week_1stday, week_start;
    long week_1stday_l;

    first_weekday = nl_langinfo(_NL_TIME_FIRST_WEEKDAY)[0];
    week_1stday_l = (long) nl_langinfo(_NL_TIME_WEEK_1STDAY);
    if (week_1stday_l == 19971130L)  // Sunday
    {
        week_1stday = 0;
    }
    else if (week_1stday_l== 19971201L) // Monday
    {
        week_1stday = 1;
    }
    else
    {
        qDebug() << Q_FUNC_INFO <<
            "nl_langinfo(_NL_TIME_WEEK_1STDAY) returned an unknown value.";
    }

    week_start = (week_1stday + first_weekday - 1) % 7;
    if (week_start == 0)
    {
        return Qt::Sunday;
    }
    else
    {
        return static_cast<Qt::DayOfWeek>(week_start);
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
