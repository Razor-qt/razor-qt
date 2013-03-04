/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2013 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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
 * END_COMMON_COPYRIGHT_HEADER */


#ifndef DEVICEACTIONNOTHING_H
#define DEVICEACTIONNOTHING_H

#include <QWidget>
#include "deviceaction.h"

class DeviceActionNothing : public DeviceAction
{
    Q_OBJECT
public:
    explicit DeviceActionNothing(RazorMountPlugin *plugin, QObject *parent=0);

protected:
    void doDeviceAdded(RazorMountDevice *device);
    void doDeviceRemoved(RazorMountDevice *device);
};

#endif // DEVICEACTIONNOTHING_H
