/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 *
 * END_COMMON_COPYRIGHT_HEADER */
/* Based on a "MountTray" project - modified for Razor needs
    http://hatred.homelinux.net

    @date   2010-11-11
    @brief  Main application class: integrate all components

    Copyright (C) 2010 by hatred <hatred@inbox.ru>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the version 2 of GNU General Public License as
    published by the Free Software Foundation.

    For more information see LICENSE and LICENSE.ru files
*/

#ifndef MOUNT_H
#define MOUNT_H

#include <QtCore>

typedef enum {
    MC_DEVICE,
    MC_DIR
} MountCheck;

typedef enum {
    CLASSIC,
    UDISKS
} MountingType;

bool diskMount(MountingType        type,
               const QString      &device,
               QString            &mount_point,
               QString            &status,
               const QString      &fs      = QString(),
               const QString      &options = QString());

bool diskUnMount(MountingType        type,
                 const QString      &device,
                 QString            &status);

/*! parse /etc/mtab and check if is the device mounted.
If is \a check MC_DEVICE \a name should be DiskInfo.device_name.
If is \a check MC_DIR \a name should be DiskInfo.name.
\retval QStringList with parsed mtab line.
*/
QStringList isMounted(const QString &name, MountCheck check);

#endif // MOUNT_H
