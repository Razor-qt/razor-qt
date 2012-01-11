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
 
#ifndef UDISKSMANAGER_H
#define UDISKSMANAGER_H

#include <QtCore/QObject>

#include "udisksinfo.h"


class UdisksManager : public QObject
{
    Q_OBJECT

public:
    UdisksManager(QObject *parent=0);
    ~UdisksManager();

    QList<UdisksInfo*> devices() { return m_devices.values(); }

signals:
    void error(const QString &msg);
    void addDevice(UdisksInfo *info);
    void removeDevice(UdisksInfo *info);

public slots:
    void addDevice(const QDBusObjectPath &device);
    void removeDevice(const QDBusObjectPath &device);
    void changeDevice(const QDBusObjectPath &device);

private:

    QHash<QString,UdisksInfo*> m_devices;
};

#endif /* _YAUD_TRAY_APP_H_ */
