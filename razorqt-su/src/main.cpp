#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <stdlib.h>

#include <sys/time.h>
#include <sys/resource.h>
#if defined(HAVE_SYS_WAIT_H)
#include <sys/wait.h>
#endif

#include <QFileInfo>
#include <QFile>
#include <QtDebug>


#include "sudialog.h"
#include "suprocess.h"


void help()
{
    qDebug() << "Usage: razor-su -u username -c command";
}


int main(int argc, char **argv )
{
    QString command;
    QString user;
    int c;
    while ((c = getopt (argc, argv, "c:u:h")) != -1)
    {
        if (c == 'c')
        {
            command = optarg;
            qDebug() << "command" << command;
            continue;
        }
        else if (c == 'u')
        {
            user = optarg;
            qDebug() << "user" << user;
            continue;
        }
        else if (c == 'h')
        {
            help();
            exit(2);
        }
    }
    QApplication app(argc, argv);
    qDebug() << app.arguments();

    // Get target uid
    QString auth_user = user;
    struct passwd *pw = getpwnam(user.toUtf8());
    if (pw == 0L)
    {
        qDebug() << "User " << user << " does not exist\n";
        exit(1);
    }
    bool other_uid = (getuid() != pw->pw_uid);
    bool change_uid = other_uid;
    if (!change_uid) {
        char *cur_user = getenv("USER");
        if (!cur_user)
            cur_user = getenv("LOGNAME");
        change_uid = (!cur_user || user != cur_user);
    }

    // Don't change uid if we're don't need to.
    if (!change_uid)
    {
        int result = system(command.toUtf8());
        result = WEXITSTATUS(result);
        return result;
    }

    // Set core dump size to 0 because we will have
    // root's password in memory.
    struct rlimit rlim;
    rlim.rlim_cur = rlim.rlim_max = 0;
    if (setrlimit(RLIMIT_CORE, &rlim))
    {
        qDebug() << "rlimit(): " << strerror(errno) << "\n";
        exit(1);
    }

    // TODO/FIXME: Check if we need a password

    // Start the Password dialog
    QString password;
    SuDialog dlg(user, auth_user, command);

    int ret = dlg.exec();
    if (ret == QDialog::Rejected)
    {
        exit(1);
    }
    password = dlg.password();


    // Some events may need to be handled (like a button animation)
    qApp->processEvents();

    // Run command
    SuProcess proc(user, password, command);
    return proc.execute();
}
