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
**  useful, but WITHOUT ANY WARRANTY; without even the baseimplied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**  See the GNU Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public License
**  along with the library.
**  If not, see <http://www.gnu.org/licenses/lgpl.html>.
**
*************/


#include <fcntl.h>
#include <unistd.h>

#include <cstddef>

#include "basestat.hpp"
#include "basestat_p.hpp"


namespace SysStat {

BaseStatPrivate::BaseStatPrivate(BaseStat* parent)
    : QObject(parent)
    , iface(parent)
    , m_timer(new QTimer(this))
    , m_synchroTimer(new QTimer(this))
{
    m_timer->setSingleShot(false);

    m_synchroTimer->setSingleShot(false);
    connect(m_synchroTimer, SIGNAL(timeout()), SLOT(synchroTimeout()));
}

BaseStatPrivate::~BaseStatPrivate()
{
}

bool BaseStatPrivate::timerIsActive(void) const
{
    return m_timer->isActive();
}

int BaseStatPrivate::updateInterval(void) const
{
    return m_timer->interval();
}

void BaseStatPrivate::setUpdateInterval(int msec)
{
    m_timer->stop();
    m_timer->setInterval(msec);
    m_synchroTimer->setInterval(msec / 10);
    if (msec > 0)
    {
        m_lastSynchro = 0;
        m_synchroTimer->start();
    }
}

void BaseStatPrivate::synchroTimeout(void)
{
    QTime now(QTime::currentTime());
    int synchro = ((now.minute() * 60 + now.second()) * 1000 + now.msec() ) / m_timer->interval();
    if ((m_lastSynchro != 0) && (m_lastSynchro != synchro))
    {
        m_synchroTimer->stop();
        m_timer->start();
    }
    m_lastSynchro = synchro;
}

void BaseStatPrivate::stopUpdating(void)
{
    m_timer->stop();
}

QString BaseStatPrivate::monitoredSource(void) const
{
    return m_source;
}

void BaseStatPrivate::setMonitoredSource(const QString &Source)
{
    m_source = Source;
}

void BaseStatPrivate::monitorDefaultSource(void)
{
    m_source = defaultSource();
}

QString BaseStatPrivate::readAllFile(const char *filename)
{
    QString result;

    static const size_t bufferSize = 1 << 12; // 4096
    static char buffer[bufferSize];

    int fd = ::open(filename, O_RDONLY);
    if (fd > 0)
    {
        ssize_t size = ::read(fd, buffer, bufferSize);
        if (size > 0)
            result = QString::fromAscii(buffer, size);
        ::close(fd);
    }

    return result;
}

QStringList BaseStatPrivate::sources(void) const
{
    return m_sources;
}

BaseStat::BaseStat(QObject *parent)
    : QObject(parent)
{
}

BaseStat::~BaseStat()
{
}

QStringList BaseStat::sources(void) const
{
    return baseimpl->sources();
}

int BaseStat::updateInterval(void) const
{
    return baseimpl->updateInterval();
}

void BaseStat::setUpdateInterval(int msec)
{
    if ((updateInterval() != msec) || (!baseimpl->timerIsActive()))
    {
        baseimpl->setUpdateInterval(msec);
        emit updateIntervalChanged(msec);
    }
}

void BaseStat::stopUpdating(void)
{
    if (updateInterval() != 0)
    {
        baseimpl->stopUpdating();
        emit updateIntervalChanged(0);
    }
}

QString BaseStat::monitoredSource(void) const
{
    return baseimpl->monitoredSource();
}

void BaseStat::setMonitoredSource(const QString &source)
{
    if (monitoredSource() != source)
    {
        baseimpl->setMonitoredSource(source);
        emit monitoredSourceChanged(source);
    }
}

void BaseStat::monitorDefaultSource(void)
{
    QString oldSource = monitoredSource();
    baseimpl->monitorDefaultSource();
    if (monitoredSource() != oldSource)
        emit monitoredSourceChanged(monitoredSource());
}

}
