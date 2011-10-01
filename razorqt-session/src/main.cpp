/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *   Christopher Regali
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include <QApplication>
#include <QIcon>

#include "razormodman.h"
#include "sessiondbusadaptor.h"

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

#ifdef DEBUG
#include <cstdio>
#include <cstdlib>
#include <QDateTime>
/*! \biref Log qDebug input to file
Used only in pure Debug builds.
*/
void dbgMessageOutput(QtMsgType type, const char *msg)
 {
    FILE *f;
    f = fopen (".razor-session.log", "a+");
    const char * dt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toUtf8();
    switch (type) {
    case QtDebugMsg:
        fprintf(f, "%s Debug: %s\n", dt, msg);
        break;
    case QtWarningMsg:
        fprintf(f, "%s Warning: %s\n", dt, msg);
        break;
    case QtCriticalMsg:
        fprintf(f, "%s Critical: %s\n", dt, msg);
        break;
    case QtFatalMsg:
        fprintf(f, "%s Fatal: %s\n", dt, msg);
        fclose(f);
        abort();
    }
    fclose(f);
}
#endif

/**
* @brief our main function doing the loading
*/
int main(int argc, char **argv)
{
#ifdef DEBUG
    qInstallMsgHandler(dbgMessageOutput);
#endif
    QApplication app(argc, argv);
    qDebug() << "Razor Session start";
    app.setWindowIcon(QIcon(QString(SHARE_DIR) + "/graphics/razor_logo.png"));

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
    QDBusConnection::sessionBus().registerService("org.razor.session");
    QDBusConnection::sessionBus().registerObject("/RazorSession", &modman);
    return app.exec();
}

