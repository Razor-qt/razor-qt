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


#include "calendar_utils.h"

#include <QtCore/QtGlobal>

#if QT_VERSION < 0x040800

#ifdef CAN_USE_BOOST

#include <boost/locale/date_time.hpp>

Qt::DayOfWeek firstDayOfWeek(void)
{
    return (boost::locale::calendar().first_day_of_week() + 6) % 7;
}

#else // use C

#include <langinfo.h>

Qt::DayOfWeek firstDayOfWeek(void)
{
    const char *const s = nl_langinfo(_NL_TIME_FIRST_WEEKDAY);

    if (s)
        if ((*s >= 1) && (*s <= 7))
            return static_cast<Qt::DayOfWeek>((*s + 6) % 7);

    return Qt::Sunday;
}

#endif

#else // use Qt >= 4.8

Qt::DayOfWeek firstDayOfWeek(void)
{
    return QLocale::system().firstDayOfWeek();
}

#endif
