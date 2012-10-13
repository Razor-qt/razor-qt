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


#include "cpufreqstat.hpp"
#include "cpufreqstat_p.hpp"


namespace SysStat {

CpuFreqStatPrivate::CpuFreqStatPrivate(CpuFreqStat *parent)
    : BaseStatPrivate(parent)
{
    m_source = defaultSource();

    connect(m_timer, SIGNAL(timeout()), SLOT(timeout()));

    updateSources();
}

CpuFreqStatPrivate::~CpuFreqStatPrivate()
{
}

void CpuFreqStatPrivate::addSource(const QString &source)
{
    bool ok;

    uint min = readAllFile(qPrintable(QString("/sys/devices/system/cpu/%1/cpufreq/scaling_max_freq").arg(source))).toUInt(&ok);
    if (ok)
    {
        uint max = readAllFile(qPrintable(QString("/sys/devices/system/cpu/%1/cpufreq/scaling_max_freq").arg(source))).toUInt(&ok);
        if (ok)
        {
            m_sources.append(source);
            m_bounds[source] = qMakePair(min, max);
        }
    }
}

void CpuFreqStatPrivate::updateSources(void)
{
    m_sources.clear();
    m_bounds.clear();

    bool ok;

    foreach (QString range, readAllFile("/sys/devices/system/cpu/online").split(QChar(','), QString::SkipEmptyParts))
    {
        int dash = range.indexOf('-');
        if (dash != -1)
        {
            uint min = range.left(dash).toUInt(&ok);
            if (ok)
            {
                uint max = range.mid(dash + 1).toUInt(&ok);
                if (ok)
                    for (uint number = min; number <= max; ++number)
                        addSource(QString("cpu%1").arg(number));
            }
        }
        else
        {
            uint number = range.toUInt(&ok);
            if (ok)
                addSource(QString("cpu%1").arg(number));
        }
    }
}

void CpuFreqStatPrivate::timeout(void)
{
    bool ok;

    uint cur = readAllFile(qPrintable(QString("/sys/devices/system/cpu/%1/cpufreq/scaling_cur_freq").arg(m_source))).toUInt(&ok);

    if (ok)
        emit update(cur);
}

QString CpuFreqStatPrivate::defaultSource(void)
{
    if (m_sources.isEmpty())
        updateSources();

    if (!m_sources.isEmpty())
        return m_sources[0];

    return QString();
}

uint CpuFreqStatPrivate::minFreq(const QString &source) const
{
    Bounds::ConstIterator I = m_bounds.constFind(source);

    if (I != m_bounds.constEnd())
        return I.value().first;

    return 0;
}

uint CpuFreqStatPrivate::maxFreq(const QString &source) const
{
    Bounds::ConstIterator I = m_bounds.constFind(source);

    if (I != m_bounds.constEnd())
        return I.value().second;

    return 0;
}

CpuFreqStat::CpuFreqStat(QObject *parent)
    : BaseStat(parent)
{
    impl = new CpuFreqStatPrivate(this);
    baseimpl = impl;

    connect(impl, SIGNAL(update(uint)), this, SIGNAL(update(uint)));
}

CpuFreqStat::~CpuFreqStat()
{
}

void CpuFreqStat::updateSources(void)
{
    dynamic_cast<CpuFreqStatPrivate*>(impl)->updateSources();
}

uint CpuFreqStat::minFreq(const QString &source) const
{
    return dynamic_cast<CpuFreqStatPrivate*>(impl)->minFreq(source);
}

uint CpuFreqStat::maxFreq(const QString &source) const
{
    return dynamic_cast<CpuFreqStatPrivate*>(impl)->maxFreq(source);
}

}
