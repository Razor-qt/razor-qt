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
