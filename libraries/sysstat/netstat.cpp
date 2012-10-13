/*************
**
** Project:      Qt-based interface to system statistics (libsysstat)
** Author:       Copyright (C) 2009 - 2012 Kuzma Shapran <Kuzma.Shapran@gmail.com>
** License:      LGPLv3
**
**  This file is part of SysStat library.
**
**  This library is free software; you can redistribute it and/or modify it
**  under the terms of the GNU Lesser General Public License as published by
**  the Free Software Foundation; either version 3 of the License, or any
**  later version. This work is distributed in the hope that it will be
**  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**  See the GNU Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public License
**  along with the library.
**  If not, see <http://www.gnu.org/licenses/lgpl.html>.
**
*************/


#include "netstat.hpp"
#include "netstat_p.hpp"


namespace SysStat {

NetStatPrivate::NetStatPrivate(NetStat *parent)
    : BaseStatPrivate(parent)
{
    m_source = defaultSource();

    connect(m_timer, SIGNAL(timeout()), SLOT(timeout()));


    QStringList rows(readAllFile("/proc/net/dev").split(QChar('\n'), QString::SkipEmptyParts));

    rows.erase(rows.begin(), rows.begin() + 2);

    foreach (QString row, rows)
    {
        QStringList tokens = row.split(QChar(':'), QString::SkipEmptyParts);
        if (tokens.size() != 2)
            continue;

        m_sources.append(tokens[0].trimmed());
    }
}

NetStatPrivate::~NetStatPrivate()
{
}

void NetStatPrivate::timeout(void)
{
    QStringList rows(readAllFile("/proc/net/dev").split(QChar('\n'), QString::SkipEmptyParts));


    if (rows.size() < 2)
        return;

    QStringList names = rows[1].split(QChar('|'));
    if (names.size() != 3)
        return;
    QStringList namesR = names[1].split(QChar(' '), QString::SkipEmptyParts);
    QStringList namesT = names[2].split(QChar(' '), QString::SkipEmptyParts);
    int receivedIndex    =                 namesR.indexOf("bytes");
    int transmittedIndex = namesR.size() + namesT.indexOf("bytes");

    rows.erase(rows.begin(), rows.begin() + 2);

    foreach (QString row, rows)
    {
        QStringList tokens = row.split(QChar(':'), QString::SkipEmptyParts);
        if (tokens.size() != 2)
            continue;

        QString interfaceName = tokens[0].trimmed();

        QStringList data = tokens[1].split(QChar(' '), QString::SkipEmptyParts);
        if (data.size() < transmittedIndex)
            continue;

        Values current;
        current.received    = data[receivedIndex   ].toULongLong();
        current.transmitted = data[transmittedIndex].toULongLong();

        if (!m_previous.contains(interfaceName))
            m_previous.insert(interfaceName, Values());
        const Values &previous = m_previous[interfaceName];

        if (interfaceName == m_source)
            emit update((( current.received - previous.received ) * 1000 ) / m_timer->interval(), (( current.transmitted - previous.transmitted ) * 1000 ) / m_timer->interval());

        m_previous[interfaceName] = current;
    }
}

QString NetStatPrivate::defaultSource(void)
{
    return "lo";
}

NetStatPrivate::Values::Values(void)
    : received(0)
    , transmitted(0)
{
}

NetStat::NetStat(QObject *parent)
    : BaseStat(parent)
{
    impl = new NetStatPrivate(this);
    baseimpl = impl;

    connect(impl, SIGNAL(update(unsigned,unsigned)), this, SIGNAL(update(unsigned,unsigned)));
}

NetStat::~NetStat()
{
}

}
