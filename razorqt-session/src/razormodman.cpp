/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL3+
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
 * version 3 of the License, or (at your option) any later version.
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

#include "razormodman.h"
#include <razorqt/razorsettings.h>
#include <razorqt/xdgautostart.h>

#include <QtDebug>
#include <QDBusInterface>
#include <QTimer>
#include <QCoreApplication>
#include "wmselectdialog.h"
#include <razorqt/xfitman.h>

/**
 * @file razormodman.cpp
 * @author Christopher "VdoP" Regali
 * @brief implements class Razormodulemanager
 */


/**
 * @brief the constructor, needs a valid modules.conf
 */
RazorModuleManager::RazorModuleManager(const QString & config, const QString & windowManager, QObject* parent)
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
    // If the WM is active do not run WM.
    if (!xfitMan().isWindowManagerActive())
    {
        QString wm;
        if (windowManager.isNull())
        {
            wm = s.value("windowmanager").toString();
            if (wm.isEmpty())
            {
                wm = showWmSelectDialog();
                s.setValue("windowmanager", wm);
                s.sync();
            }
            //qDebug() << "Using window manager from config file" << wm;
        }
        else
        {
            wm = windowManager;
            //qDebug() << "Using window manager specified with command line" << windowManager;
        }

        QProcess * wmProcess = new QProcess(this);
        wmProcess->start(wm);
        connect(wmProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
                this, SLOT(logout()));
    }
    // window manager

    // modules
    s.beginGroup("modules");
    foreach(QString i, s.childKeys())
    {
        bool start = s.value(i, true).toBool();
        if (!start)
            continue;

        QProcess* tmp = new QProcess(this);
        tmp->start(i);
        // we have to ensure modules start before we'll process
        // autostart (some apps may require tray for example)
        tmp->waitForStarted();

        procMap[i] = tmp;

        connect(tmp, SIGNAL(finished(int, QProcess::ExitStatus)),
                this, SLOT(restartModules(int, QProcess::ExitStatus)));
    }
    s.endGroup();

    // HACK: ensure all is prepared and running for 3rd party apps.
    //       There is always some chance to start an app when eg. panel won't be ready yet.
    //       I faced this issue with qsynergy (petr).
    sleep(2); // a guess-what constant...

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

QString RazorModuleManager::showWmSelectDialog()
{
    WmSelectDialog dlg;
    dlg.exec();
    return dlg.windowManager();
}

void razor_setenv(const char *env, const QByteArray &value)
{
    qDebug() << "Environment variable" << env << "=" << value;
    qputenv(env, value);
}

void razor_setenv_prepend(const char *env, const QByteArray &value, const QByteArray &separator)
{
    QByteArray orig(qgetenv(env));
    orig = orig.prepend(separator);
    orig = orig.prepend(value);
    qDebug() << "Setting special" << env << " variable:" << orig;
    razor_setenv(env, orig);
}

