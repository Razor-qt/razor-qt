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


#ifndef LIBSYSSTAT__CPU_STAT__INCLUDED
#define LIBSYSSTAT__CPU_STAT__INCLUDED


#include <QtCore/QObject>

#include <sysstat/basestat.hpp>


namespace SysStat {

class CpuStatPrivate;

class SYSSTATSHARED_EXPORT CpuStat : public BaseStat
{
    Q_OBJECT

    Q_ENUMS(Monitoring)

public:
    enum Monitoring { LoadAndFrequency, LoadOnly, FrequencyOnly };

public:
    CpuStat(QObject *parent = NULL);
    ~CpuStat();

    void updateSources(void);

    uint minFreq(const QString &source) const;
    uint maxFreq(const QString &source) const;

signals:
    void update(float user, float nice, float system, float other, float frequencyRate, uint frequency);
    void update(float user, float nice, float system, float other);
    void update(uint frequency);

    void monitoringChanged(Monitoring);

public:
    Q_PROPERTY(Monitoring monitoring READ monitoring WRITE setMonitoring NOTIFY monitoringChanged)

public slots:
    Monitoring monitoring(void) const;
    void setMonitoring(Monitoring value);

protected:
    CpuStatPrivate* impl;
};

}

#endif //LIBSYSSTAT__CPU_STAT__INCLUDED
