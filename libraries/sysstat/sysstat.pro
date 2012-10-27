##############
##
##  This file is part of SysStat library.
##
##  SysStat is a Qt-based interface to system statistics
##
##  Authors:
##       Copyright (c) 2009 - 2012 Kuzma Shapran <Kuzma.Shapran@gmail.com>
##
##  Licenses:  LGPL2+, LGPLv3
##
##  This library is free software; you can redistribute it and/or modify it
##  under the terms of the GNU Lesser General Public License as published by
##  the Free Software Foundation;
##  when it is used together with Razor-qt (http://razor-qt.org):
##   either version 2.1 of the License, or (at your option) any later version
##  otherwise:
##   either version 3 of the License, or (at your option) any later version.
##
##  This work is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
##  See the GNU Lesser General Public License for more details.
##
##  You should have received a copy of the GNU Lesser General Public License
##  along with the library.
##  If not, write to the Free Software Foundation, Inc.,
##  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA .
##  Or see <http://www.gnu.org/licenses/lgpl.html>.
##
##############

TARGET = sysstat

DEFINES += SYSSTAT_LIBRARY

TEMPLATE = lib

QT -= gui

isEmpty(MAJOR_VERSION):MAJOR_VERSION = 1
isEmpty(MINOR_VERSION):MINOR_VERSION = 0

VERSION = $${MAJOR_VERSION}.$${MINOR_VERSION}.0

INCLUDEPATH += ..

DEFINES += \
    MAJOR_VERSION=$${MAJOR_VERSION} \
    MINOR_VERSION=$${MINOR_VERSION}

SOURCES += \
    version.cpp \
    basestat.cpp \
    cpustat.cpp \
    memstat.cpp \
    netstat.cpp

PUBLIC_HEADERS += \
    version.hpp \
    sysstat_global.hpp \
    basestat.hpp \
    cpustat.hpp \
    memstat.hpp \
    netstat.hpp

PRIVATE_HEADERS += \
    version_p.hpp \
    basestat_p.hpp \
    cpustat_p.hpp \
    memstat_p.hpp \
    netstat_p.hpp

HEADERS += \
    $${PUBLIC_HEADERS} \
    $${PRIVATE_HEADERS}

OTHER_FILES += \
    COPYING
