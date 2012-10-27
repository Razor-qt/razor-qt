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

    QString readAllFile(const char *filename);

    QTimer *mTimer;
    QTimer *mSynchroTimer;
    QString mSource;
    QStringList mSources;

    int mLastSynchro;
};

}

#endif //LIBSYSSTAT__BASE_STAT__PRIVATE__INCLUDED
