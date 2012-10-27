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


#ifndef LIBSYSSTAT__VERSION__PRIVATE__INCLUDED
#define LIBSYSSTAT__VERSION__PRIVATE__INCLUDED


#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define MAJOR_VERSION_STR TOSTRING(MAJOR_VERSION)
#define MINOR_VERSION_STR TOSTRING(MINOR_VERSION)
#define PATCH_VERSION_STR TOSTRING(PATCH_VERSION)


#endif // LIBSYSSTAT__VERSION__PRIVATE__INCLUDED
