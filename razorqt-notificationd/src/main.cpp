/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include <QtGui/QApplication>
#include <QtDBus/QDBusConnection>

#include "notifyadaptor.h"
#include "notifyd.h"


int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    a.setStyleSheet(
                "NotificationArea {background: transparent;}"
                "NotificationLayout {background: transparent;}"
                "Notification { border: 1px solid #00f000; background-color: #f0f0f0; margin: 0px;}"
                   );
    
    Notifyd* demo = new Notifyd();
    new NotifyAdaptor(demo);

    QDBusConnection connection = QDBusConnection::sessionBus();
    bool ret = connection.registerService("org.freedesktop.Notifications");
    if (!ret)
        qDebug() << "registerService failed :(";
    ret = connection.registerObject("/org/freedesktop/Notifications", demo);
    if (!ret)
        qDebug() << "registerObject failed :(";

    qDebug() << "exec!";


    return a.exec();
}
