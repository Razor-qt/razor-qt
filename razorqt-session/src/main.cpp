/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
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

#include <razorqt/razorapplication.h>
#include <QtGui/QIcon>

#include "razormodman.h"
#include "sessiondbusadaptor.h"
#include "razortranslate.h"

/**
* @file main.cpp
* @author Christopher "VdoP" Regali
* @brief just starts the sub-apps and in future maybe saves the windowstates

razor-session can be called as is (without any argument) - it will start
"failback" session (sessionf.conf). When there will be used -c foo argument
razor-session -c foo
it will use foo.conf. Currently there are launchers for windowmanagers:
session-openbox.conf
session-eggwm.conf
*/

/**
* @brief our main function doing the loading
*/
int main(int argc, char **argv)
{
    RazorApplication app(argc, argv);
    //qDebug() << "Razor Session start";

    TRANSLATE_APP;

    char* session = NULL;
    char* winmanager = NULL;
    int c;
    while ((c = getopt (argc, argv, "c:w:")) != -1)
    {
        if (c == 'c')
        {
            session = optarg;
            break;
        }
        else if (c == 'w')
        {
            winmanager = optarg;
            break;
        }
    }
    
    // PATH for out own bundled XDG tools
    razor_setenv_prepend("PATH", PATH_PREPEND);
    // special variable for Razor environment menu
    razor_setenv("XDG_MENU_PREFIX", "razor-");

    RazorModuleManager modman(session, winmanager);
    new SessionDBusAdaptor(&modman);
    // connect to D-Bus and register as an object:
    QDBusConnection::sessionBus().registerService("org.razorqt.session");
    QDBusConnection::sessionBus().registerObject("/RazorSession", &modman);
    return app.exec();
}

