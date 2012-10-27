/*************
**
**  This file is part of SysStat library.
**
**  SysStat is a Qt-based interface to system statistics
**
**  Authors:
**       Copyright (c) 2009 - 2012 Kuzma Shapran <Kuzma.Shapran@gmail.com>
**
**  Licenses:  LGPL2+, LGPLv3
**
**  This library is free software; you can redistribute it and/or modify it
**  under the terms of the GNU Lesser General Public License as published by
**  the Free Software Foundation;
**  when it is used as a part of Razor-qt (http://razor-qt.org):
**   either version 2.1 of the License, or (at your option) any later version
**  otherwise:
**   either version 3 of the License, or (at your option) any later version.
**
**  This work is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**  See the GNU Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public License
**  along with the library.
**  If not, write to the Free Software Foundation, Inc.,
**  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA .
**  Or see <http://www.gnu.org/licenses/lgpl.html>.
**
*************/


#include "cpustat.hpp"
#include "cpustat_p.hpp"


namespace SysStat {

CpuStatPrivate::CpuStatPrivate(CpuStat *parent)
    : BaseStatPrivate(parent)
    , mMonitoring(CpuStat::LoadAndFrequency)
{
    mSource = defaultSource();

    connect(mTimer, SIGNAL(timeout()), SLOT(timeout()));

    updateSources();
}

void CpuStatPrivate::addSource(const QString &source)
{
    bool ok;

    uint min = readAllFile(qPrintable(QString("/sys/devices/system/cpu/%1/cpufreq/scaling_min_freq").arg(source))).toUInt(&ok);
    if (ok)
    {
        uint max = readAllFile(qPrintable(QString("/sys/devices/system/cpu/%1/cpufreq/scaling_max_freq").arg(source))).toUInt(&ok);
        if (ok)
            mBounds[source] = qMakePair(min, max);
    }
}

void CpuStatPrivate::updateSources(void)
{
    mSources.clear();

    foreach (QString row, readAllFile("/proc/stat").split(QChar('\n'), QString::SkipEmptyParts))
    {
        QStringList tokens = row.split(QChar(' '), QString::SkipEmptyParts);
        if( (tokens.size() < 5)
        || (!tokens[0].startsWith("cpu")) )
            continue;

        mSources.append(tokens[0]);
    }

    mBounds.clear();

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

CpuStatPrivate::~CpuStatPrivate()
{
}

void CpuStatPrivate::timeout(void)
{
    if ( (mMonitoring == CpuStat::LoadOnly)
      || (mMonitoring == CpuStat::LoadAndFrequency) )
    {
        foreach (QString row, readAllFile("/proc/stat").split(QChar('\n'), QString::SkipEmptyParts))
        {
            QStringList tokens = row.split(QChar(' '), QString::SkipEmptyParts);
            if ( (tokens.size() < 5)
            || (!tokens[0].startsWith("cpu")) )
                continue;

            const QString &cpuName = tokens[0];

            Values current;
            current.user   = tokens[1].toULongLong();
            current.nice   = tokens[2].toULongLong();
            current.system = tokens[3].toULongLong();
            current.idle   = tokens[4].toULongLong();
            current.other  = 0;
            for (int i = 5; i < tokens.size(); ++i)
                current.other += tokens[i].toULongLong();
            current.sum();

            if (!mPrevious.contains(cpuName))
                mPrevious.insert(cpuName, Values());
            const Values &previous = mPrevious[cpuName];

            if (cpuName == mSource)
            {
                float sumDelta = static_cast<float>(current.total - previous.total);

                if (mMonitoring == CpuStat::LoadAndFrequency)
                {
                    bool ok;

                    float freqRate = 1.0;
                    uint freq = 0;

                    if (mSource == "cpu")
                    {
                        uint count = 0;
                        freqRate = 0.0;

                        for (Bounds::ConstIterator I = mBounds.constBegin(); I != mBounds.constEnd(); ++I)
                        {
                            uint thisFreq = readAllFile(qPrintable(QString("/sys/devices/system/cpu/%1/cpufreq/scaling_cur_freq").arg(I.key()))).toUInt(&ok);

                            if (ok)
                            {
                                freq += thisFreq;
                                freqRate += static_cast<float>(thisFreq) / static_cast<float>(I.value().second);
                                ++count;
                            }
                        }
                        if (!count)
                            freqRate = 1.0;
                        else
                        {
                            freq /= count;
                            freqRate /= count;
                        }
                    }
                    else
                    {
                        freq = readAllFile(qPrintable(QString("/sys/devices/system/cpu/%1/cpufreq/scaling_cur_freq").arg(mSource))).toUInt(&ok);

                        if (ok)
                        {
                            Bounds::ConstIterator I = mBounds.constFind(mSource);
                            if (I != mBounds.constEnd())
                                freqRate = static_cast<float>(freq) / static_cast<float>(I.value().second);
                        }
                    }

                    emit update(
                        static_cast<float>(current.user   - previous.user  ) / sumDelta,
                        static_cast<float>(current.nice   - previous.nice  ) / sumDelta,
                        static_cast<float>(current.system - previous.system) / sumDelta,
                        static_cast<float>(current.other  - previous.other ) / sumDelta,
                        freqRate,
                        freq);
                }
                else
                {
                    emit update(
                        static_cast<float>(current.user   - previous.user  ) / sumDelta,
                        static_cast<float>(current.nice   - previous.nice  ) / sumDelta,
                        static_cast<float>(current.system - previous.system) / sumDelta,
                        static_cast<float>(current.other  - previous.other ) / sumDelta);
                }
            }

            mPrevious[cpuName] = current;
        }
    }
    else
    {
        bool ok;
        uint freq = 0;

        if (mSource == "cpu")
        {
            uint count = 0;

            for (Bounds::ConstIterator I = mBounds.constBegin(); I != mBounds.constEnd(); ++I)
            {
                uint thisFreq = readAllFile(qPrintable(QString("/sys/devices/system/cpu/%1/cpufreq/scaling_cur_freq").arg(I.key()))).toUInt(&ok);

                if (ok)
                {
                    freq += thisFreq;
                    ++count;
                }
            }
            if (count)
            {
                freq /= count;
            }
        }
        else
        {
            freq = readAllFile(qPrintable(QString("/sys/devices/system/cpu/%1/cpufreq/scaling_cur_freq").arg(mSource))).toUInt(&ok);
        }
        emit update(freq);
    }
}

QString CpuStatPrivate::defaultSource(void)
{
    return "cpu";
}

CpuStatPrivate::Values::Values(void)
    : user(0)
    , nice(0)
    , system(0)
    , idle(0)
    , other(0)
    , total(0)
{
}

void CpuStatPrivate::Values::sum(void)
{
    total = user + nice + system + idle + other;
}

CpuStat::Monitoring CpuStatPrivate::monitoring(void) const
{
    return mMonitoring;
}

void CpuStatPrivate::setMonitoring(CpuStat::Monitoring value)
{
    mMonitoring = value;
}

CpuStat::CpuStat(QObject *parent)
    : BaseStat(parent)
{
    impl = new CpuStatPrivate(this);
    baseimpl = impl;

    connect(impl, SIGNAL(update(float,float,float,float,float,uint)), this, SIGNAL(update(float,float,float,float,float,uint)));
    connect(impl, SIGNAL(update(float,float,float,float)), this, SIGNAL(update(float,float,float,float)));
    connect(impl, SIGNAL(update(uint)), this, SIGNAL(update(uint)));
}

CpuStat::~CpuStat()
{
}

void CpuStat::updateSources(void)
{
    dynamic_cast<CpuStatPrivate*>(impl)->updateSources();
}

CpuStat::Monitoring CpuStat::monitoring(void) const
{
    return impl->monitoring();
}

void CpuStat::setMonitoring(CpuStat::Monitoring value)
{
    if (impl->monitoring() != value)
    {
        impl->setMonitoring(value);
        emit monitoringChanged(value);
    }
}

}
