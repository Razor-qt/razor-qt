/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
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
 * END_COMMON_COPYRIGHT_HEADER */
 
#include "udisksmanager.h"


UdisksManager::UdisksManager(QObject *parent)
    : QObject(parent)
{
    QDBusConnection system = QDBusConnection::systemBus();

    if (!system.isConnected())
    {
        emit error(tr("Cannot connect to Udisks daemon"));
    }

    system.connect("org.freedesktop.UDisks",
                    "/org/freedesktop/UDisks",
                    "org.freedesktop.UDisks",
                    "DeviceAdded",
                    this,
                    SLOT(onDeviceAdded(QDBusObjectPath)));
    system.connect("org.freedesktop.UDisks",
                    "/org/freedesktop/UDisks",
                    "org.freedesktop.UDisks",
                    "DeviceRemoved",
                    this,
                    SLOT(onDeviceRemoved(QDBusObjectPath)));
    system.connect("org.freedesktop.UDisks",
                    "/org/freedesktop/UDisks",
                    "org.freedesktop.UDisks",
                    "DeviceChanged",
                    this,
                    SLOT(onDeviceChanged(QDBusObjectPath)));

    QDBusInterface devEnum("org.freedesktop.UDisks",
                           "/org/freedesktop/UDisks",
                           "org.freedesktop.UDisks",
                           QDBusConnection::systemBus());

    QDBusMessage enumRes = devEnum.call("EnumerateDevices");
    // TODO/FIXME: error checking
//    if (enumRes.type() == QDBusMessage::ErrorMessage)
//    {
//        fprintf(stderr, "ERROR: Can't call EnumerateDevices\n");
//        fprintf(stderr, "       %s : %s\n", qPrintable(enumRes.errorName()), qPrintable(enumRes.errorMessage()));
//    }
//
//    if (enumRes.type() != QDBusMessage::ReplyMessage || !enumRes.arguments().at(0).canConvert<QDBusArgument>())
//    {
//        fprintf(stderr, "ERROR: Unexpected result type of EnumerateDevices call\n");
//    }
//
    const QDBusArgument enumArg = enumRes.arguments().at(0).value<QDBusArgument>();
//    if (enumArg.currentType() != QDBusArgument::ArrayType)
//    {
//        fprintf(stderr, "ERROR: Unexpected argument type of EnumerateDevices call\n");
//    }

    enumArg.beginArray();
    while (!enumArg.atEnd())
    {
        addDevice(qdbus_cast<QDBusObjectPath>(enumArg));
    }
    enumArg.endArray();
}

UdisksManager::~UdisksManager()
{
    qDeleteAll(m_devices);
}

void UdisksManager::addDevice(const QDBusObjectPath &device)
{
    UdisksInfo *info = new UdisksInfo(device);
    if (!info->isExternal())
    {
        info->deleteLater();
        return;
    }
    
    if (m_devices.contains(info->udisksPath()))
        removeDevice(device);

    m_devices[info->udisksPath()] = info;
    emit addDevice(info);
}

void UdisksManager::removeDevice(const QDBusObjectPath &device)
{
    UdisksInfo *info = m_devices.take(device.path());
    if (info)
    {
        emit removeDevice(info);
        info->deleteLater();
    }
}

void UdisksManager::changeDevice(const QDBusObjectPath &device)
{
    UdisksInfo *info = m_devices.take(device.path());
    if (!info)
    {
        addDevice(device);
    }
    else
    {
        removeDevice(device);
        addDevice(device);
    }
}
