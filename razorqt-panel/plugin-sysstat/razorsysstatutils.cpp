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

#include <QtCore/QRegExp>
#include <QtCore/qmath.h>

#include "razorsysstatutils.h"


namespace PluginSysStat
{

QString netSpeedToString(int value)
{
    QString prefix;
    static const char prefixes[] = "kMG";
    if (value / 10)
        prefix = QChar(prefixes[value / 10 - 1]);

    return QString("%1 %2B/s").arg(1 << (value % 10)).arg(prefix);
}

int netSpeedFromString(QString value)
{
    QRegExp re("^(\\d+) ([kMG])B/s$");
    if (re.exactMatch(value))
    {
        int shift = 0;
        switch (re.cap(2)[0].toAscii())
        {
        case 'k':
            shift = 10;
            break;

        case 'M':
            shift = 20;
            break;

        case 'G':
            shift = 30;
            break;
        }

        return qCeil(qLn(re.cap(1).toInt()) / qLn(2.)) + shift;
    }

    return 0;
}

}
