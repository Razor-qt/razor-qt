/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
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

#include "razormodman.h"
#include <razorqt/razorsettings.h>
#include <qtxdg/xdgautostart.h>
#include <unistd.h>

#include <QtCore/QtDebug>
#include <QtDBus/QDBusInterface>
#include <QtCore/QCoreApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QSystemTrayIcon>
#include "wmselectdialog.h"
#include <razorqt/xfitman.h>
#include "windowmanager.h"
#include <wordexp.h>

#define MAX_CRASHES_PER_APP 5

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

        // Wait until the WM loads
        int waitCnt = 300;
        while (!xfitMan().isWindowManagerActive() && waitCnt)
        {
            qDebug() << "******************** Wait until the WM loads" << waitCnt;
            waitCnt--;
            usleep(100000);
        }
    }

    // XDG autostart
    XdgDesktopFileList fileList = XdgAutoStart::desktopFileList();
    QList<XdgDesktopFile*> trayApps;
    for (XdgDesktopFileList::iterator i = fileList.begin(); i != fileList.end(); ++i)
    {
        if (i->value("X-Razor-Need-Tray", false).toBool())
            trayApps.append(&(*i));
        else
            startProcess(*i);
    }

    if (!trayApps.isEmpty())
    {
        int waitCnt = 200;
        while (!QSystemTrayIcon::isSystemTrayAvailable())
        {
            qDebug() << "******************** Wait for tray" << waitCnt;
            if (!waitCnt)
            {
                qWarning() << "******************** No systray implementation started in session. Continuing.";
                break;
            }
            waitCnt--;
            usleep(100000);
        }

        foreach (XdgDesktopFile* f, trayApps)
            startProcess(*f);
    }

    m_crashTimer.setInterval(60000);
    connect(&m_crashTimer, SIGNAL(timeout()), this, SLOT(resetCrashReport()));
}

void RazorModuleManager::startProcess(const XdgDesktopFile& file)
{
    if (!file.value("X-Razor-Module", false).toBool())
    {
        file.startDetached();
        return;
    }
    QStringList args = file.expandExecString();
    QString command = args.takeFirst();
    QProcess* proc = new QProcess(this);
    proc->start(command, args);

    mProcMap[proc] = new XdgDesktopFile(file);

    connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(restartModules(int, QProcess::ExitStatus)));
}

void RazorModuleManager::restartModules(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "void RazorModuleManager::restartModules() called and it's wrong. Something is failing" << sender();
    QProcess * proc = qobject_cast<QProcess*>(sender());
    Q_ASSERT(proc);
    XdgDesktopFile* procFile = mProcMap[proc];
    QString procName = procFile->name();

    switch (exitStatus)
    {
        case QProcess::NormalExit:
            qDebug() << "Process" << procName << "(" << proc << ") exited correctly.";
            break;
        case QProcess::CrashExit:
        {
            qDebug() << "Process" << procName << "(" << proc << ") has to be restarted";
            if (!m_crashReport.contains(proc))
                m_crashReport[proc] = 0;
            int stat = m_crashReport[proc]++;
            if (stat >= MAX_CRASHES_PER_APP)
            {
                QMessageBox::warning(0, tr("Razor Session Crash Report"),
                                    tr("Application '%1' crashed too many times. Its autorestart has been disabled for current session.").arg(procName));
                delete mProcMap.take(proc);
                proc->deleteLater();
            }
            else
            {
                QStringList args = procFile->expandExecString();
                QString command = args.takeFirst();
                proc->start(command, args);
            }
            break;
        }
    }
}


RazorModuleManager::~RazorModuleManager()
{
    delete m_power;
    qDeleteAll(mProcMap);
}

/**
* @brief this logs us out by terminating our session
**/
void RazorModuleManager::logout()
{
    // modules
    ModulesMapIterator i(mProcMap);
    while (i.hasNext())
    {
        i.next();
        qDebug() << "Module logout" << i.key();
        QProcess * p = i.key();
        p->blockSignals(true);
        p->terminate();
        if (!p->waitForFinished())
            qDebug() << "Module" << p << "rejected to close correctly. Kill it down.";
    }
    QCoreApplication::exit(0);
}

QString RazorModuleManager::showWmSelectDialog()
{
    WindowManagerList availableWM = getWindowManagerList(true);
    if (availableWM.count() == 1)
        return availableWM.at(0).command;

    WmSelectDialog dlg(availableWM);
    dlg.exec();
    return dlg.windowManager();
}

void RazorModuleManager::resetCrashReport()
{
    m_crashReport.clear();
}

void razor_setenv(const char *env, const QByteArray &value)
{
    wordexp_t p;
    wordexp(value, &p, 0);
    if (p.we_wordc == 1)
    {

        qDebug() << "Environment variable" << env << "=" << p.we_wordv[0];
        qputenv(env, p.we_wordv[0]);
    }
    else
    {
        qWarning() << "Error expanding environment variable" << env << "=" << value;
        qputenv(env, value);
    }
     wordfree(&p);
}

void razor_setenv_prepend(const char *env, const QByteArray &value, const QByteArray &separator)
{
    QByteArray orig(qgetenv(env));
    orig = orig.prepend(separator);
    orig = orig.prepend(value);
    qDebug() << "Setting special" << env << " variable:" << orig;
    razor_setenv(env, orig);
}

