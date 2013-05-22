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
#include <qtxdg/xdgdirs.h>
#include <unistd.h>

#include <QtCore/QtDebug>
#include <QtCore/QCoreApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QSystemTrayIcon>
#include <QtCore/QFileInfo>
#include <QFileSystemWatcher>
#include <QDateTime>
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
      mConfig(config),
      mWindowManager(windowManager),
      mWmProcess(new QProcess(this)),
      mThemeWatcher(new QFileSystemWatcher(this))
{
    if (mConfig.isEmpty())
        mConfig = "session";

    connect(mThemeWatcher, SIGNAL(directoryChanged(QString)), SLOT(themeFolderChanged(QString)));

    connect(RazorSettings::globalSettings(), SIGNAL(razorThemeChanged()), SLOT(themeChanged()));

    // Wait until the event loop starts
    QTimer::singleShot(0, this, SLOT(startup()));
}

void RazorModuleManager::startup()
{
    qDebug() << __FILE__ << ":" << __LINE__ << "Session" << mConfig << "about to launch (default 'session')";

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

    // The razor-confupdate can update the settings of the WM, so run it first.
    startConfUpdate();

    // Start window manager
    startWm(&s);

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
        int waitCnt = 600;
        while (!QSystemTrayIcon::isSystemTrayAvailable())
        {
//            qDebug() << "******************** Wait for tray" << waitCnt;
            if (!waitCnt)
            {
                qWarning() << "******************** No systray implementation started in session. Continuing.";
                break;
            }
            waitCnt--;
            QCoreApplication::processEvents();
            usleep(100000);
        }

        foreach (XdgDesktopFile* f, trayApps)
            startProcess(*f);
    }


    QStringList paths;
    paths << XdgDirs::dataHome(false);
    paths << XdgDirs::dataDirs();

    foreach(QString path, paths)
    {
        QFileInfo fi(QString("%1/razor/themes").arg(path));
        if (fi.exists())
            mThemeWatcher->addPath(fi.absoluteFilePath());
    }

    themeChanged();
}

void RazorModuleManager::themeFolderChanged(const QString& /*path*/)
{
    QString newTheme;
    if (!QFileInfo(mCurrentThemePath).exists())
    {
        const QList<RazorTheme> &allThemes = razorTheme.allThemes();
        if (!allThemes.isEmpty())
        {
            newTheme = allThemes[0].name();
        }
        else
        {
            return;
        }
    }
    else
    {
        newTheme = razorTheme.currentTheme().name();
    }

    RazorSettings settings("razor");
    if (newTheme == settings.value("theme"))
    { // force the same theme to be updated
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
        settings.setValue("__theme_updated__", QDateTime::currentDateTime().toTime_t() * 1000);
#else
        settings.setValue("__theme_updated__", QDateTime::currentMSecsSinceEpoch());
#endif
    }
    else
    {
        settings.setValue("theme", newTheme);
    }
    sync();
}

void RazorModuleManager::themeChanged()
{
    if (!mCurrentThemePath.isEmpty())
        mThemeWatcher->removePath(mCurrentThemePath);

    if (razorTheme.currentTheme().isValid())
    {
        mCurrentThemePath = razorTheme.currentTheme().path();
        mThemeWatcher->addPath(mCurrentThemePath);
    }
}

void RazorModuleManager::startWm(RazorSettings *settings)
{
    // If the WM is active do not run WM.
    if (xfitMan().isWindowManagerActive())
        return;

    if (mWindowManager.isEmpty())
    {
        mWindowManager = settings->value("windowmanager").toString();
    }

    // If previuos WM was removed, we show dialog.
    if (mWindowManager.isEmpty() || ! findProgram(mWindowManager.split(' ')[0]))
    {
        mWindowManager = showWmSelectDialog();
        settings->setValue("windowmanager", mWindowManager);
        settings->sync();

    }

    mWmProcess->start(mWindowManager);

    // Wait until the WM loads
    int waitCnt = 300;
    while (!xfitMan().isWindowManagerActive() && waitCnt)
    {
        waitCnt--;
        QCoreApplication::processEvents();
        usleep(100000);
    }
}

void RazorModuleManager::startProcess(const XdgDesktopFile& file)
{
    if (!file.value("X-Razor-Module", false).toBool())
    {
        file.startDetached();
        return;
    }
    QStringList args = file.expandExecString();
    if (args.isEmpty())
    {
        qWarning() << "Wrong desktop file" << file.fileName();
        return;
    }
    RazorModule* proc = new RazorModule(file, this);
    connect(proc, SIGNAL(moduleStateChanged(QString,bool)), this, SIGNAL(moduleStateChanged(QString,bool)));
    proc->start();

    QString name = QFileInfo(file.fileName()).fileName();
    mNameMap[name] = proc;

    connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(restartModules(int, QProcess::ExitStatus)));
}

void RazorModuleManager::startProcess(const QString& name)
{
    if (!mNameMap.contains(name))
    {
        foreach (const XdgDesktopFile& file, XdgAutoStart::desktopFileList(false))
        {
            if (QFileInfo(file.fileName()).fileName() == name)
            {
                startProcess(file);
                return;
            }
        }
    }
}

void RazorModuleManager::stopProcess(const QString& name)
{
    if (mNameMap.contains(name))
        mNameMap[name]->terminate();
}

QStringList RazorModuleManager::listModules() const
{
    return QStringList(mNameMap.keys());
}

void RazorModuleManager::startConfUpdate()
{
    XdgDesktopFile desktop(XdgDesktopFile::ApplicationType, ":razor-confupdate", "razor-confupdate --watch");
    desktop.setValue("Name", "Razor config updater");
    desktop.setValue("X-Razor-Module", true);
    startProcess(desktop);
}

void RazorModuleManager::restartModules(int exitCode, QProcess::ExitStatus exitStatus)
{
    RazorModule* proc = qobject_cast<RazorModule*>(sender());
    Q_ASSERT(proc);

    if (!proc->isTerminating())
    {
        QString procName = proc->file.name();
        switch (exitStatus)
        {
            case QProcess::NormalExit:
                qDebug() << "Process" << procName << "(" << proc << ") exited correctly.";
                break;
            case QProcess::CrashExit:
            {
                qDebug() << "Process" << procName << "(" << proc << ") has to be restarted";
                time_t now = time(NULL);
                mCrashReport[proc].prepend(now);
                while (now - mCrashReport[proc].back() > 60)
                    mCrashReport[proc].pop_back();
                if (mCrashReport[proc].length() >= MAX_CRASHES_PER_APP)
                {
                    QMessageBox::warning(0, tr("Razor Session Crash Report"),
                                        tr("Application '%1' crashed too many times. Its autorestart has been disabled for current session.").arg(procName));
                }
                else
                {
                    proc->start();
                    return;
                }
                break;
            }
        }
    }
    mNameMap.remove(proc->fileName);
    proc->deleteLater();
}


RazorModuleManager::~RazorModuleManager()
{
    qDeleteAll(mNameMap);
    delete mWmProcess;
}

/**
* @brief this logs us out by terminating our session
**/
void RazorModuleManager::logout()
{
    // modules
    ModulesMapIterator i(mNameMap);
    while (i.hasNext())
    {
        i.next();
        qDebug() << "Module logout" << i.key();
        RazorModule* p = i.value();
        p->terminate();
    }
    i.toFront();
    while (i.hasNext())
    {
        i.next();
        RazorModule* p = i.value();
        if (p->state() != QProcess::NotRunning && !p->waitForFinished())
        {
            qWarning() << QString("Module '%1' won't terminate ... killing.").arg(i.key());
            p->kill();
        }
    }

    mWmProcess->terminate();
    if (mWmProcess->state() != QProcess::NotRunning && !mWmProcess->waitForFinished())
    {
        qWarning() << QString("Window Manager won't terminate ... killing.");
        mWmProcess->kill();
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
    mCrashReport.clear();
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

RazorModule::RazorModule(const XdgDesktopFile& file, QObject* parent) :
    QProcess(parent),
    file(file),
    fileName(QFileInfo(file.fileName()).fileName()),
    mIsTerminating(false)
{
    connect(this, SIGNAL(stateChanged(QProcess::ProcessState)), SLOT(updateState(QProcess::ProcessState)));
}

void RazorModule::start()
{
    mIsTerminating = false;
    QStringList args = file.expandExecString();
    QString command = args.takeFirst();
    QProcess::start(command, args);
}

void RazorModule::terminate()
{
    mIsTerminating = true;
    QProcess::terminate();
}

bool RazorModule::isTerminating()
{
    return mIsTerminating;
}

void RazorModule::updateState(QProcess::ProcessState newState)
{
    if (newState != QProcess::Starting)
        emit moduleStateChanged(fileName, (newState == QProcess::Running));
}
