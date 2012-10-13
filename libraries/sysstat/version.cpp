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


#include <QtCore/QString>
#include "version_p.hpp"


namespace SysStat {

namespace version {

QString verbose(void)
{
    return QString("%1.%2.%3").arg(MAJOR_VERSION_STR).arg(MINOR_VERSION_STR).arg(PATCH_VERSION_STR);
}

int major(void)
{
    return MAJOR_VERSION;
}

int minor(void)
{
    return MINOR_VERSION;
}

int patch(void)
{
    return PATCH_VERSION;
}

}

}
