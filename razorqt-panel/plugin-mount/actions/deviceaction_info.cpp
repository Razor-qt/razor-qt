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


#include "deviceaction_info.h"
#include "../razormountplugin.h"
#include <razorqt/razornotification.h>
#include <QDebug>

DeviceActionInfo::DeviceActionInfo(RazorMountPlugin *plugin, QObject *parent):
    DeviceAction(plugin, parent)
{
    qDebug() << Q_FUNC_INFO;
}


void DeviceActionInfo::doDeviceAdded(RazorMountDevice *device)
{
    qDebug() << Q_FUNC_INFO;

    showMessage(tr("The device <b><nobr>\"%1\"</nobr></b> is connected.").arg(device->label()));
}


void DeviceActionInfo::doDeviceRemoved(RazorMountDevice *device)
{
    qDebug() << Q_FUNC_INFO;

    showMessage(tr("The device <b><nobr>\"%1\"</nobr></b> is removed.").arg(device->label()));
}


void DeviceActionInfo::showMessage(const QString &text)
{
    RazorNotification::notify(tr("Removable media/devices manager"),
                              text
                          #if QT_VERSION >= 0x040700
                              , mPlugin->icon().name()
                          #endif
                              );
}

