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


#include <fcntl.h>
#include <unistd.h>

#include <cstddef>

#include "basestat.hpp"
#include "basestat_p.hpp"


namespace SysStat {

BaseStatPrivate::BaseStatPrivate(BaseStat* parent)
    : QObject(parent)
    , mTimer(new QTimer(this))
    , mSynchroTimer(new QTimer(this))
{
    mTimer->setSingleShot(false);

    mSynchroTimer->setSingleShot(false);
    connect(mSynchroTimer, SIGNAL(timeout()), SLOT(synchroTimeout()));
}

BaseStatPrivate::~BaseStatPrivate()
{
}

bool BaseStatPrivate::timerIsActive(void) const
{
    return mTimer->isActive();
}

int BaseStatPrivate::updateInterval(void) const
{
    return mTimer->interval();
}

void BaseStatPrivate::setUpdateInterval(int msec)
{
    mTimer->stop();
    mTimer->setInterval(msec);
    mSynchroTimer->setInterval(msec / 10);
    if (msec > 0)
    {
        mLastSynchro = 0;
        mSynchroTimer->start();
    }
}

void BaseStatPrivate::synchroTimeout(void)
{
    QTime now(QTime::currentTime());
    int synchro = ((now.minute() * 60 + now.second()) * 1000 + now.msec() ) / mTimer->interval();
    if ((mLastSynchro != 0) && (mLastSynchro != synchro))
    {
        mSynchroTimer->stop();
        mTimer->start();
    }
    mLastSynchro = synchro;
}

void BaseStatPrivate::stopUpdating(void)
{
    mTimer->stop();
}

QString BaseStatPrivate::monitoredSource(void) const
{
    return mSource;
}

void BaseStatPrivate::setMonitoredSource(const QString &Source)
{
    mSource = Source;
}

void BaseStatPrivate::monitorDefaultSource(void)
{
    mSource = defaultSource();
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
    return mSources;
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
