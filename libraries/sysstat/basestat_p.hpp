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


#ifndef LIBSYSSTAT__BASE_STAT__PRIVATE__INCLUDED
#define LIBSYSSTAT__BASE_STAT__PRIVATE__INCLUDED


#include <QtCore/QObject>
#include <QtCore/QtGlobal>
#include <QtCore/QTimer>
#include <QtCore/QTime>
#include <QtCore/QStringList>

#include "cpustat.hpp"


namespace SysStat {

class BaseStatPrivate : public QObject
{
    Q_OBJECT

public:
    BaseStatPrivate(BaseStat *parent = NULL);
    ~BaseStatPrivate();

    QStringList sources(void) const;

    bool timerIsActive(void) const;
    int updateInterval(void) const;
    void setUpdateInterval(int msec);
    void stopUpdating(void);

    QString monitoredSource(void) const;
    void setMonitoredSource(const QString &Source);
    void monitorDefaultSource(void);

private slots:
    void synchroTimeout(void);

protected:
    virtual QString defaultSource(void) = 0;

    BaseStat * const iface;

    QString readAllFile(const char *filename);

    QTimer *m_timer;
    QTimer *m_synchroTimer;
    QString m_source;
    QStringList m_sources;

    int m_lastSynchro;
};

}

#endif //LIBSYSSTAT__BASE_STAT__PRIVATE__INCLUDED
