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


#ifndef LIBSYSSTAT__CPU_FREQ_STAT__PRIVATE__INCLUDED
#define LIBSYSSTAT__CPU_FREQ_STAT__PRIVATE__INCLUDED


#include <QtCore/QObject>
#include <QtCore/QtGlobal>
#include <QtCore/QMap>
#include <QtCore/QPair>

#include "basestat_p.hpp"
#include "cpufreqstat.hpp"


namespace SysStat {

class CpuFreqStatPrivate : public BaseStatPrivate
{
    Q_OBJECT

public:
    CpuFreqStatPrivate(CpuFreqStat *parent = NULL);
    ~CpuFreqStatPrivate();

    void updateSources(void);

    uint minFreq(const QString &source) const;
    uint maxFreq(const QString &source) const;

signals:
    void update(uint freq);

private slots:
    void timeout(void);

private:
    QString defaultSource(void);

    typedef QMap<QString, QPair<uint, uint> > Bounds;
    Bounds m_bounds;

    void addSource(const QString &source);
};

}

#endif //LIBSYSSTAT__CPU_FREQ_STAT__PRIVATE__INCLUDED
