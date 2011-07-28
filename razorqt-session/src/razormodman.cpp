/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
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

#include "razormodman.h"
#include <razorqt/razorsettings.h>
#include <razorqt/xdgautostart.h>

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
RazorModuleManager::RazorModuleManager(const QString & config, QObject* parent)
    : QObject(parent),
    mConfig(config)
{
    m_power = new QDBusInterface("org.freedesktop.Hal", "/org/freedesktop/Hal/devices/computer",
                               "org.freedesktop.Hal.Device.SystemPowerManagement",
                               QDBusConnection::systemBus());

    qDebug() << __FILE__ << ":" << __LINE__ << "Session" << config << "about to launch (deafult 'session')";
    if (mConfig.isEmpty())
        mConfig = "session";


    RazorSettings s(mConfig);

    // first - set some user defined environment variables (like TERM...)
    s.beginGroup("environment");
    QByteArray envVal;
    foreach (QString i, s.childKeys())
    {
        envVal = s.value(i).toByteArray();
        razor_setenv(i.toUtf8().constData(), envVal);
    }
    s.endGroup();

    // then rest of the config:
    
    // window manager
    QString wm = s.value("windowmanager", "openbox").toString();
    QProcess * wmProcess = new QProcess(this);
    wmProcess->start(wm);
    connect(wmProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(logout()));
    
    // modules
    s.beginGroup("modules");
    foreach(QString i, s.childKeys())
    {
        bool start = s.value(i, true).toBool();
        if (!start)
            continue;

        QProcess* tmp = new QProcess(this);
        tmp->start(i);
        procMap[i] = tmp;

        connect(tmp, SIGNAL(finished(int, QProcess::ExitStatus)),
                this, SLOT(restartModules(int, QProcess::ExitStatus)));
    }
    s.endGroup();

    // start 3rd party apps without restart handling
    int count = s.beginReadArray("autostart");
    QString cmd;
    for (int i = 0; i < count; ++i)
    {
        s.setArrayIndex(i);
        cmd = s.value("exec", "").toString();
        if (cmd.isEmpty())
        {
            qDebug() << __FILE__ << ":" << __LINE__ << "empty name for module. Skipping.";
            continue;
        }
        QProcess* tmp = new QProcess(this);
        tmp->start(cmd);
        procMap[cmd] = tmp;
    }
    s.endArray();
    
    // XDG autostart
    XdgAutoStart xdgautostart;
    foreach (XdgDesktopFile* f, xdgautostart.list())
    {
        f->startDetached();
    }
}

void RazorModuleManager::restartModules(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "void RazorModuleManager::restartModules() called and it's wrong. Something is failing" << sender();
    QProcess * proc = qobject_cast<QProcess*>(sender());
    Q_ASSERT(proc);
    QString procName;

    ModulesMapIterator i(procMap);
    while (i.hasNext())
    {
        i.next();
        if (i.value() == proc)
        {
            procName = i.key();
            break;
        }
    }

    switch (exitStatus)
    {
        case QProcess::NormalExit:
            qDebug() << "Process" << procName << "(" << proc << ") exited correctly.";
            break;
        case QProcess::CrashExit:
            qDebug() << "Process" << procName << "(" << proc << ") has to be restarted";
            proc->start(procName);
            break;
    }

}


RazorModuleManager::~RazorModuleManager()
{
    delete m_power;
}

/**
* @brief this logs us out by terminating our session
**/
void RazorModuleManager::logout()
{
    // modules
    ModulesMapIterator i(procMap);
    while (i.hasNext())
    {
        i.next();
        qDebug() << "Module logout" << i.key();
        QProcess * p = i.value();
        p->blockSignals(true);
        p->terminate();
        if (!p->waitForFinished())
            qDebug() << "Module" << p << "rejected to close correctly. Kill it down.";
    }
    QCoreApplication::exit(0);
}

void razor_setenv(const char *env, const QByteArray &value)
{
    qDebug() << "Environment variable" << env << "=" << value;
    qputenv(env, value);
}

void razor_setenv_prepend(const char *env, const QByteArray &value, const QByteArray &separator)
{
    QByteArray orig(qgetenv("PATH"));
    orig = orig.prepend(separator);
    orig = orig.prepend(value);
    qDebug() << "Setting special" << env << " variable:" << orig;
    razor_setenv(env, orig);
}

