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


#include "memstat.hpp"
#include "memstat_p.hpp"


namespace SysStat {

MemStatPrivate::MemStatPrivate(MemStat *parent)
    : BaseStatPrivate(parent)
{
    m_source = defaultSource();

    connect(m_timer, SIGNAL(timeout()), SLOT(timeout()));

    m_sources << "memory" << "swap";
}

MemStatPrivate::~MemStatPrivate()
{
}

void MemStatPrivate::timeout(void)
{
    qulonglong memTotal;
    qulonglong memFree;
    qulonglong memBuffers;
    qulonglong memCached;
    qulonglong swapTotal;
    qulonglong swapFree;

    foreach (QString row, readAllFile("/proc/meminfo").split(QChar('\n'), QString::SkipEmptyParts))
    {
        QStringList tokens = row.split(QChar(' '), QString::SkipEmptyParts);
        if (tokens.size() != 3)
            continue;

        if (tokens[0] == "MemTotal:")
            memTotal = tokens[1].toULong();
        else if(tokens[0] == "MemFree:")
            memFree = tokens[1].toULong();
        else if(tokens[0] == "Buffers:")
            memBuffers = tokens[1].toULong();
        else if(tokens[0] == "Cached:")
            memCached = tokens[1].toULong();
        else if(tokens[0] == "SwapTotal:")
            swapTotal = tokens[1].toULong();
        else if(tokens[0] == "SwapFree:")
            swapFree = tokens[1].toULong();
    }

    if (m_source == "memory")
    {
        if (memTotal)
        {
            float memTotal_d     = static_cast<float>(memTotal);
            float applications_d = static_cast<float>(memTotal - memFree - memBuffers - memCached) / memTotal_d;
            float buffers_d      = static_cast<float>(memBuffers) / memTotal_d;
            float cached_d       = static_cast<float>(memCached) / memTotal_d;

            emit memoryUpdate(applications_d, buffers_d, cached_d);
        }
    }
    else if (m_source == "swap")
    {
        if (swapTotal)
        {
            float swapUsed_d = static_cast<float>(swapTotal - swapFree) / static_cast<float>(swapTotal);

            emit swapUpdate(swapUsed_d);
        }
    }
}

QString MemStatPrivate::defaultSource(void)
{
    return "memory";
}

MemStat::MemStat(QObject *parent)
    : BaseStat(parent)
{
    impl = new MemStatPrivate(this);
    baseimpl = impl;

    connect(impl, SIGNAL(memoryUpdate(float,float,float)), this, SIGNAL(memoryUpdate(float,float,float)));
    connect(impl, SIGNAL(swapUpdate(float)), this, SIGNAL(swapUpdate(float)));
}

MemStat::~MemStat()
{
}

}
