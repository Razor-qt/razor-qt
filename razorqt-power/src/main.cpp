/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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
#include "dialog.h"
#include "razortranslate.h"
#include <QtCore/QTextStream>
#include <razorqt/razorpower/razorpower.h>
#include <QtCore/QDebug>

void help()
{
    QTextStream out(stdout);
    out << "Usage: razor-power" << endl;
    out << "  or:  razor-power --check ACTION" << endl;
    out << "  or:  razor-power ACTION" << endl;

    out << "The first variant starts the GUI interface." << endl;
    out << "The second variant checks is the action can be performed." << endl;
    out << "The third variant executes the ACTION without the GUI." << endl;

    out << endl;
    out << "Actions:" << endl;
    out << "  logout            Close the current user session." << endl;
    out << "  hibernate         Hibernate the comuter" << endl;
    out << "  reboot            Reboot the comuter" << endl;
    out << "  shutdown, halt    Shutdown the comuter" << endl;
    out << "  suspend           Suspend the comuter" << endl;

}

int main(int argc, char *argv[])
{
    RazorApplication app(argc, argv);
    bool check=false;
    bool actionSet=false;
    RazorPower::Action action;

    QStringList args = app.arguments();
    for (int i=1; i < args.count(); ++i)
    {
        QString arg = args.at(i);
        if (arg == "--help" || arg == "-h" )
        {
            help();
            return 0;
        }

        if (arg == "--check" )
        {
            check = true;
            continue;
        }

        arg = arg.toLower();

        if (arg == "logout" )    { action = RazorPower::PowerLogout;    actionSet = true;  continue; }
        if (arg == "hibernate" ) { action = RazorPower::PowerHibernate; actionSet = true;  continue; }
        if (arg == "reboot" )    { action = RazorPower::PowerReboot;    actionSet = true;  continue; }
        if (arg == "shutdown" )  { action = RazorPower::PowerShutdown;  actionSet = true;  continue; }
        if (arg == "halt" )      { action = RazorPower::PowerShutdown;  actionSet = true;  continue; }
        if (arg == "suspend" )   { action = RazorPower::PowerSuspend;   actionSet = true;  continue; }

        qWarning() << "Unknown option: " << args.at(i);
        help();
        return 2;
    }

    if (actionSet)
    {
        RazorPower power;
        bool res;

        if (check)
            res = power.canAction(action);
        else
            res = power.doAction(action);

        if (res)
            return 0;
        else
            return 1;
    }
    {
        TRANSLATE_APP;

        Dialog w;
        w.show();
        return app.exec();
    }
}
