/* BEGIN_COMMON_COPYRIGHT_HEADER
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
 
#include "suprocess.h"

#include <QProcess>
#include <QtDebug>
#include <QMessageBox>


SuProcess::SuProcess(const QString & username, const QString & password, const QString & command)
    : m_username(username),
      m_password(password),
      m_command(command)
{
}

int SuProcess::execute()
{
    qDebug() << "SuProcess execute:" << m_username << m_command;
    QProcess proc;
    QStringList args;
    
    args << "-c" << m_command << m_username;
    
    proc.start("su", args);
    qDebug() << "starting... with args:" << args;
    
    if (!proc.waitForStarted())
    {
        qDebug() << "waitForStarted failed";
        return 666;
    }

    qDebug() << "writting pass";
    proc.write(m_password.toUtf8());
    proc.closeWriteChannel();

    if (!proc.waitForFinished(-1))
    {
        qDebug() << "waitForFinished failed";
        return 666;
    }

    qDebug() << "Exit code:" << proc.exitCode();
    int ret = proc.exitCode();
    if (ret == 1)
    {
        QMessageBox::warning(0, tr("su error"), tr("'su' failed itself. Wrong password probably."));
    }
    else if (ret == 126 || ret == 127)
    {
        QMessageBox::warning(0, tr("su error"), tr("Subshell cannot be found or subshell cannot be invoked."));
    }
    
    return ret;
}
