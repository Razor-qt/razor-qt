#include <QApplication>

#include "razormodman.h"

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

#if DEBUG
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
#if DEBUG
    qInstallMsgHandler(dbgMessageOutput);
#endif
    QApplication app(argc, argv);

    char* session = NULL;
    int c;
    while ((c = getopt (argc, argv, "c:")) != -1)
    {
        if (c == 'c')
        {
            session = optarg;
            break;
        }
    }

    RazorModuleManager modman(session);
    return app.exec();
}
