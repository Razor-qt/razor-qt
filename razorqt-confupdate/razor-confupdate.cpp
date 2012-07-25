/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This code based on kconf_update utility.
 *   Copyright (c) 2001 Waldo Bastian <bastian@kde.org>
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

#include "rzupdate.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>

#include "rzwatcher.h"

void printHelp()
{
    QTextStream out(stdout);
    out << "Usage: razor-confupdate [options] [file]" << endl;
    out << endl;
    out << "Razor-qt tool for updating user configuration files" << endl;
    out << endl;
    out << "Generic options:" << endl;
    out << "  -h, --help                Show help about options" << endl;
    out << "  --version                 Show version information" << endl;
    out << "  --watch                   Do not exit, but watch a new update files." << endl;
    out << "  --                        End of options" << endl;
    out << endl;
    out << "Options:" << endl;
    out << "  --debug                   Keep output results from scripts" << endl;
    out << "  --check <update-file>     Check whether config file itself requires updating" << endl;
    //out << "  -v, --verbose             Explain what is being done" << endl;
    out << endl;
    out << "Arguments:" << endl;
    out << "  file                      File to read update instructions from" << endl;
}

void printVersion()
{
    QTextStream out(stdout);
    out << "razor-confupdate " << "0.5" << endl;
    out << "Copyright (c) 2012 Razor team" << endl;
    out << endl;
    out << "License LGPLv2+: GNU Lesser GPL version 2 or later <http://www.gnu.org/licenses/lgpl-2.1.html>." << endl;
    out << "This is free software: you are free to change and redistribute it." << endl;
    out << "There is NO WARRANTY, to the extent permitted by law." << endl;
    out << endl;
    out << "Program based on kconf_update utility." << endl;
    out << "Copyright (c) 2001 Waldo Bastian <bastian@kde.org>, <bastian@suse.com>" << endl;

}

void printError(const QString &msg)
{
    QTextStream out(stdout);
    out << msg;
    out << "Use --help to get a list of available command line options.";
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    RzUpdate *rzUpdate = new RzUpdate();

    bool debug = false;
    bool watch = false;
    bool processOptions = true;
    QStringList files;
    QStringList args = app.arguments();
    for (int i=1; i < args.count(); ++i)
    {
        QString arg = args.at(i);
        if (processOptions)
        {
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

            if (arg == "--watch")
            {
                watch = true;
                continue;
            }

            if (arg == "--debug")
            {
                debug = true;
                continue;
            }

            if (arg == "--check")
            {
                if (i+1 < args.count())
                {
                    rzUpdate->setArgCheckFile(args.at(i+1));
                    i++;
                    continue;
                }
                else
                {
                    printError("'<update-file>' missing.");
                    continue;
                }
            }

            if (arg == "--")
            {
                processOptions = false;
                continue;
            }
        }
        files << args.at(i);
    }

    rzUpdate->setDebug(debug);
    rzUpdate->run(files);
    delete rzUpdate;

    if (watch)
    {
        RzWatcher watcher(debug);
        app.exec();
    }
    return 0;
}
