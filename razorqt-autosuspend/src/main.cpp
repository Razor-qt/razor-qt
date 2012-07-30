/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Christian Surlykke <christian@surlykke.dk>
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

#include <QDBusConnection>
#include <QDebug>

#include <razorqt/razorapplication.h>
#include "razortranslate.h"
#include "razorautosuspend.h"

int main(int argc, char *argv[])
{

    RazorApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    TRANSLATE_APP;

    // To ensure only one instance of razor-autosuspend is running we register as a DBus service and refuse to run
    // if not able to do so.
    // We do not register any object as we don't have any dbus-operations to expose.g
    if (! QDBusConnection::sessionBus().registerService("org.razor-qt.razor-autosuspend"))
    {
        qWarning() << "Unable to register 'org.razor-qt.razor-autosuspend' service - is another instance of razor-autosuspend running?";
        return 1;
    }
    else
    {
        RazorAutosuspendd razorAutosuspendd;
        return a.exec();
    }
}
