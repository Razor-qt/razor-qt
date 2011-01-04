#include "razormodman.h"
#include <razorqt/readsettings.h>

#include <QtDebug>
#include <QDBusInterface>
#include <QTimer>
#include <QCoreApplication>

/**
 * @file razormodman.cpp
 * @author Christopher "VdoP" Regali
 * @brief implements class Razormodulemanager
 */


/**
 * @brief the constructor, needs a valid modules.conf
 */
RazorModuleManager::RazorModuleManager(QObject* parent)
    : QObject(parent)
{
    power = new QDBusInterface("org.freedesktop.Hal", "/org/freedesktop/Hal/devices/computer",
                               "org.freedesktop.Hal.Device.SystemPowerManagement",
                               QDBusConnection::systemBus());

    cfg = new ReadSettings("session", this);

    QSettings * s = cfg->settings();
    autorestart = s->value("autorestart", true).toBool();

    int count = s->beginReadArray("modules");
    QString cmd;
    for (int i = 0; i < count; ++i)
    {
        s->setArrayIndex(i);
        cmd = s->value("exec", "").toString();
        if (cmd.isEmpty())
        {
            qDebug() << __FILE__ << ":" << __LINE__ << "empty name for module. Skipping.";
            continue;
        }
        bool power = s->value("doespower", false).toBool();

        QProcess* tmp = new QProcess(this);
        tmp->start(cmd);
        if (power)
        {
            connect(tmp, SIGNAL(readyRead()), this, SLOT(parseState()));
        }

        connect(tmp, SIGNAL(finished(int, QProcess::ExitStatus)),
                this,SLOT(restartModules(int, QProcess::ExitStatus)));
        Module m;
        m.power = power;
        m.process = tmp;
        procMap[cmd] = m;
    }
    s->endArray();

    int delay = s->value("autostart_delay", 1).toInt();
    QTimer::singleShot(delay * 1000, this, SLOT(autoStartSingleShot()));
}

void RazorModuleManager::autoStartSingleShot()
{
    QSettings * s = cfg->settings();
    int count = s->beginReadArray("autostart");
    QString cmd;
    for (int i = 0; i < count; ++i)
    {
        s->setArrayIndex(i);
        cmd = s->value("exec", "").toString();
        if (cmd.isEmpty())
        {
            qDebug() << __FILE__ << ":" << __LINE__ << "empty name for module. Skipping.";
            continue;
        }
        QProcess* tmp = new QProcess(this);
        tmp->start(cmd);
        autostartList << tmp;
    }
    s->endArray();
}

void RazorModuleManager::restartModules(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "void RazorModuleManager::restartModules() called and it's wrong. Something is failing";
    QProcess * proc = qobject_cast<QProcess*>(sender());
    Q_ASSERT(proc);
    QString procName;
    bool power = false;

    ModulesMapIterator i(procMap);
    while (i.hasNext())
    {
        i.next();
        if (i.value().process == proc)
        {
            procName = i.key();
            power = i.value().power;
            break;
        }
    }

    switch (exitStatus)
    {
        case QProcess::NormalExit:
            if (! power)
            {
                logout();
            }
            break;
        case QProcess::CrashExit:
            if (autorestart)
            {
                qDebug() << "Process" << procName << "(" << proc << ") has to be restarted";
                proc->start(procName);
            }
            break;
    }

}

/**
* @brief parses the output of the Process for RAZOR_DO commands
*/
void RazorModuleManager::parseState()
{
    qDebug() << "parsestate!";
    QProcess * p = qobject_cast<QProcess*>(sender());
    Q_ASSERT(p);
    doOperation(p->readLine());
}

RazorModuleManager::~RazorModuleManager()
{
    delete power;
}

void RazorModuleManager::doOperation(const QString  & _cmd)
{
    QString cmd(_cmd.trimmed());
    qDebug() << "Razorstate: got output: " << cmd;
    if (cmd =="RAZOR_DO_LOGOUT")
        logout();
    else if (cmd == "RAZOR_DO_SHUTDOWN")
        shutdown();
    else if (cmd == "RAZOR_DO_REBOOT")
        reboot();
}

/**
* @brief this logs us out by terminating our session
**/
void RazorModuleManager::logout()
{
    // autostart
    foreach (QProcess * p, autostartList)
    {
        p->blockSignals(true);
        p->terminate();
        if (!p->waitForFinished())
            qDebug() << "Autostart" << p << "rejected to close correctly. Kill it down.";
        delete p;
    }
    // modules
    ModulesMapIterator i(procMap);
    while (i.hasNext())
    {
        i.next();
        QProcess * p = i.value().process;
        p->blockSignals(true);
        p->terminate();
        if (!p->waitForFinished())
            qDebug() << "Autostart" << p << "rejected to close correctly. Kill it down.";
    }
    QCoreApplication::exit(0);
}

/**
* @brief reboot via Dbus
**/

void RazorModuleManager::reboot()
{
    power->call("Reboot");
}

/**
* @brief shutdown via Dbus
**/

void RazorModuleManager::shutdown()
{
    //qDebug() << "Would have shut down now";
    //debug!! no real shutdown while testing :)
    power->call("Shutdown");
}
