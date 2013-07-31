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


#include <QtGui/QApplication>
#include <QtGui/QIcon>
#include <QtCore/QDebug>
#include <QtCore/QLibraryInfo>
#include <QDirIterator>
#include <csignal>

#include "razorpanelapplication.h"
#include "razorpanel.h"
#include "razortranslate.h"

/*! The razor-panel is the panel of Razor-qt.
  Usage: razor-panel [CONFIG_ID]
    CONFIG_ID      Section name in config file ~/.razor/panel.conf
                   (default main)
 */

void termSignalHandler(int)
{
    qApp->quit();
}


void printHelp()
{
    QTextStream out(stdout);
    out << "Usage: razor-panel [options]" << endl;
    out << endl;
    out << "Start Razor-qt panel and its plugins" << endl;
    out << endl;
    out << "Options:" << endl;
    out << "  -h, --help                    Show help about options" << endl;
    out << "  --version                     Show version information" << endl;
    out << "  -c, --configfile=CONFIGFILE   Use alternate configuration file" << endl;
}


void printVersion()
{
    QTextStream out(stdout);
    out << "razor-confupdate " << RAZOR_VERSION << endl;
    out << "Copyright (c) 2012 Razor team" << endl;
    out << endl;
    out << "License LGPLv2+: GNU Lesser GPL version 2 or later <http://www.gnu.org/licenses/lgpl-2.1.html>." << endl;
    out << "This is free software: you are free to change and redistribute it." << endl;
    out << "There is NO WARRANTY, to the extent permitted by law." << endl;
}


int main(int argc, char *argv[])
{
    QString configFile;
    for (int i=1; i < argc; ++i)
    {
        QString arg = QString::fromLocal8Bit(argv[i]);

        if (arg == "--help" || arg == "-h")
        {
            printHelp();
            return 0;
        }

        if (arg == "--version")
        {
            printVersion();
            return 0;
        }

        if (arg == "-c" || arg.startsWith("--conf"))
        {
            if (i+1 < argc)
            {
                configFile = QString::fromLocal8Bit(argv[i+1]);
            }
        }
    }

    RazorPanelApplication *app = new RazorPanelApplication(argc, argv, configFile);

    TRANSLATE_APP;

    //Setup Unix signal handlers
    struct sigaction term;
    term.sa_handler = termSignalHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags |= SA_RESTART;
    sigaction(SIGTERM, &term, 0);
    sigaction(SIGINT,  &term, 0);

    bool res = app->exec();

    app->deleteLater();
    return res;
}
