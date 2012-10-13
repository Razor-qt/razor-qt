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


#ifndef LIBSYSSTAT__NET_STAT__PRIVATE__INCLUDED
#define LIBSYSSTAT__NET_STAT__PRIVATE__INCLUDED


#include <QtCore/QObject>
#include <QtCore/QtGlobal>
#include <QtCore/QMap>

#include "basestat_p.hpp"
#include "netstat.hpp"


namespace SysStat {

class NetStatPrivate : public BaseStatPrivate
{
    Q_OBJECT

public:
    NetStatPrivate(NetStat *parent = NULL);
    ~NetStatPrivate();

signals:
    void update(unsigned received, unsigned transmitted);

private slots:
    void timeout(void);

private:
    QString defaultSource(void);

    typedef struct Values
    {
        Values(void);

        qulonglong received;
        qulonglong transmitted;
    } Values;
    typedef QMap<QString, Values> NamedValues;
    NamedValues m_previous;
};

}

#endif //LIBSYSSTAT__NET_STAT__PRIVATE__INCLUDED
