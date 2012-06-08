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

#include "razorqt/razorapplication.h"

#include "notifyadaptor.h"
#include "notifyd.h"


int main(int argc, char** argv)
{
    RazorApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    // Ensure the helper widgets are hidden
    a.setStyleSheet(a.styleSheet() +
                "NotificationArea {background: transparent;}"
                "NotificationLayout {background: transparent;}"
                   );
    
    Notifyd* daemon = new Notifyd();
    new NotifyAdaptor(daemon);

    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.registerService("org.freedesktop.Notifications"))
        qDebug() << "registerService failed: another service with 'org.freedesktop.Notifications' runs already";
    if (!connection.registerObject("/org/freedesktop/Notifications", daemon))
        qDebug() << "registerObject failed: another object with '/org/freedesktop/Notifications' runs already";


    return a.exec();
}
