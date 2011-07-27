/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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

#include <QtCore/QProcess>
#include <QtGui/QMessageBox>

#include <qtxdg/xdgicon.h>
#include "screensaver.h"


ScreenSaver::ScreenSaver(QObject * parent)
    : QObject(parent)
{
    m_xdgProcess = new QProcess(this);
    connect(m_xdgProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(xdgProcess_finished(int,QProcess::ExitStatus)));
}

QList<QAction*> ScreenSaver::availableActions()
{
    QList<QAction*> ret;
    QAction * act;

    act = new QAction(XdgIcon::fromTheme("system-lock-screen"), tr("Lock Screen"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(lockScreen()));
    ret.append(act);

    return ret;
}

void ScreenSaver::lockScreen()
{
    m_xdgProcess->start("xdg-screensaver", QStringList() << "activate");
}

void ScreenSaver::xdgProcess_finished(int err, QProcess::ExitStatus status)
{
    if (status == QProcess::CrashExit)
    {
        QMessageBox::warning(0,
                             tr("Screen Saver Activation Error"),
                             tr("An error occurred starting screensaver. "
                                "xdg-screensaver cannot be started due its crash")
                            );
    }
    else if (err == -2)
    {
        QMessageBox::warning(0,  
                             tr("Screen Saver Activation Error"),
                             tr("An error occurred starting screensaver. "
                                "xdg-screensaver is not installed correctly.")
                            );  
    }
    else if (err == -1)
    {
        QMessageBox::warning(0,
                             tr("Screen Saver Activation Error"),
                             tr("An error occurred starting screensaver. "
                                "xdg-screensaver cannot be started")
                            );
    }
    else if (err == 0)
    {
        emit activated();
    }
    else if (err == 1)
    {
        QMessageBox::warning(0,  
                             tr("Screen Saver Activation Error"),
                             tr("An error occurred starting screensaver. "
                                "Syntax error in xdg-screensaver arguments.")
                            );
    }
    else if (err == 3)
    {
        QMessageBox::warning(0,  
                             tr("Screen Saver Activation Error"),
                             tr("An error occurred starting screensaver. "
                                "Ensure you have xscreensaver installed and running.")
                            );
    }
    else if (err == 4)
    {
        QMessageBox::warning(0,  
                             tr("Screen Saver Activation Error"),
                             tr("An error occurred starting screensaver. "
                                "Action 'activate' failed. "
                                "Ensure you have xscreensaver installed and running.")
                            );
    }
    else
    {
        QMessageBox::warning(0,  
                             tr("Screen Saver Activation Error"),
                             tr("An error occurred starting screensaver. "
                                "Unkown error - undocumented return value from xdg-screensaver=%1.").arg(err)
                            );
    }
}

