/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2013 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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

#include <QCoreApplication>

#include <QString>
#include <QStringList>

#include "meta_types.h"
#include "core.h"

#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <syslog.h>


#define DEFAULT_CONFIG ".config/razor/globalkeyshortcuts.conf"


int main(int argc, char *argv[])
{
    bool wrongArgs = false;
    bool printHelp = false;
    bool runAsDaemon = false;
    bool useSyslog = false;
    bool minLogLevelSet = false;
    int minLogLevel = LOG_NOTICE;
    bool multipleActionsBehaviourSet = false;
    MultipleActionsBehaviour multipleActionsBehaviour = MULTIPLE_ACTIONS_BEHAVIOUR_FIRST;
    QStringList configFiles;

    static struct option longOptions[] =
    {
        {"no-daemon", no_argument, 0, 'n'},
        {"daemon", no_argument, 0, 'd'},
        {"use-syslog", no_argument, 0, 's'},
        {"log-level", required_argument, 0, 'l'},
        {"multiple-actions-behaviour", required_argument, 0, 'm'},
        {"config-file", required_argument, 0, 'f'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    for (;;)
    {
        int optionIndex = 0;

        int c = getopt_long(argc, argv, "h?", longOptions, &optionIndex);

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
        case 'n':
            runAsDaemon = false;
            break;

        case 'd':
            runAsDaemon = true;
            break;

        case 's':
            useSyslog = true;
            break;

        case 'l':
            if (!strcmp(optarg, "error"))
            {
                minLogLevel = LOG_ERR;
            }
            else if (!strcmp(optarg, "warning"))
            {
                minLogLevel = LOG_WARNING;
            }
            else if (!strcmp(optarg, "notice"))
            {
                minLogLevel = LOG_NOTICE;
            }
            else if (!strcmp(optarg, "info"))
            {
                minLogLevel = LOG_INFO;
            }
            else if (!strcmp(optarg, "debug"))
            {
                minLogLevel = LOG_DEBUG;
            }
            else
            {
                fprintf(stderr, "Invalid minimal log level: %s\n", optarg);
                wrongArgs = true;
                printHelp = true;
                break;
            }
            minLogLevelSet = true;
            break;

        case 'm':
            if (!strcmp(optarg, "first"))
            {
                multipleActionsBehaviour = MULTIPLE_ACTIONS_BEHAVIOUR_FIRST;
            }
            else if (!strcmp(optarg, "last"))
            {
                multipleActionsBehaviour = MULTIPLE_ACTIONS_BEHAVIOUR_LAST;
            }
            else if (!strcmp(optarg, "all"))
            {
                multipleActionsBehaviour = MULTIPLE_ACTIONS_BEHAVIOUR_ALL;
            }
            else if (!strcmp(optarg, "none"))
            {
                multipleActionsBehaviour = MULTIPLE_ACTIONS_BEHAVIOUR_NONE;
            }
            else
            {
                fprintf(stderr, "Invalid multiple actions behaviour: %s\n", optarg);
                wrongArgs = true;
                printHelp = true;
                break;
            }
            multipleActionsBehaviourSet = true;
            break;

        case 'f':
            configFiles.push_back(QString::fromLocal8Bit(optarg));
            break;

        case '?':
        case 'h':
            printHelp = true;
            break;

        default:
            wrongArgs = true;
            printHelp = true;
        }
    }

    if (printHelp)
    {
        printf("Global key shortcuts daemon\n"
               "\n"
               "Version: " RAZOR_VERSION "\n"
               "License: GNU Lesser General Public License version 2.1 or later\n"
               "Copyright: (c) 2013 Razor team\n"
               "\n"
               "Usage %s [OPTIONS]\n"
               "\n"
               "Possible options are:\n"
               "\n"
               "  --no-daemon\n"
               "      Run as a usual application, not a daemon\n"
               "      and print messages to stderr.\n"
               "\n"
               "  --daemon\n"
               "      Run as a daemon, not a usual application\n"
               "      and print messages to syslog.\n"
               "\n"
               "  --use-syslog\n"
               "      Print messages to syslog if run as a usual application.\n"
               "\n"
               "  --log-level=VALUE\n"
               "      Set minimal log level.\n"
               "      Possible values are:\n"
               "          error\n"
               "          warning\n"
               "          notice (default)\n"
               "          info\n"
               "          debug .\n"
               "\n"
               "  --multiple-actions-behaviour=VALUE\n"
               "      Set the behaviour for the case of multiple actions\n"
               "      assigned to the same shortcut.\n"
               "      Possible values are:\n"
               "          first (default)\n"
               "          last\n"
               "          all\n"
               "          none .\n"
               "\n"
               "  --config-file=FILENAME\n"
               "      Use config file FILENAME. Can be used several times.\n"
               "      The last loaded file is used to save settings.\n"
               "      Default is: ${HOME}/" DEFAULT_CONFIG "\n"
               "\n"
               "  --help\n"
               "  -h\n"
               "  -?\n"
               "      This help.\n"
               , basename(argv[0]));
        return wrongArgs ? EXIT_FAILURE : EXIT_SUCCESS;
    }

    if (runAsDaemon)
    {
        if (daemon(0, 0) < 0)
        {
            fprintf(stderr, "Cannot become a daemon: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
    }

    int ignoreIt = chdir("/");
    (void)ignoreIt;

    if (configFiles.empty())
    {
        configFiles.push_back(QString::fromLocal8Bit(getenv("HOME")) + "/" DEFAULT_CONFIG);
    }

    QCoreApplication app(argc, argv);

    Core core(runAsDaemon || useSyslog, minLogLevelSet, minLogLevel, configFiles, multipleActionsBehaviourSet, multipleActionsBehaviour);

    if (!core.ready())
    {
        return EXIT_FAILURE;
    }

    return app.exec();
}
