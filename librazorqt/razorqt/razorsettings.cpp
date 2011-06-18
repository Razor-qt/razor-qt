/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
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

#include "razorsettings.h"
#include <QDebug>
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QtCore/QStringList>

#define RAZOR_HOME_CFG QDir::homePath() + "/.razor/"


class RazorSettingsPrivate
{
public:
    RazorSettingsPrivate(RazorSettings* parent):
        mParent(parent)
    {
    }

private:
    RazorSettings* mParent;
};


/************************************************
 Create parent directories, if necessary
 ************************************************/
void createDir(const QString& file)
{
    QDir dir(QFileInfo(file).absoluteDir());

    if (!dir.exists())
        dir.mkpath(".");


}


/************************************************
  This looks in all the usual system paths for our file
 ************************************************/
QString getSysPath(const QString& module)
{
    QStringList paths;

#ifdef SHARE_DIR
    // test for non-standard install dirs - useful for development for example
    paths << SHARE_DIR;

#else
    #warning SHARE_DIR is not defined. Config will not be searched in the CMAKE_INSTALL_PREFIX
    // this is ugly and it should be used only in a very special cases
    // standard cmake build contains the SHARE_DIR defined in the top level CMakeLists.txt
    //
    // /usr/local/ goes first as it's usually before the /usr in the PATH etc.
    paths << "/usr/local/share/razor/";
    paths << "/usr/share/razor/";
#endif

    foreach(QString path, paths)
    {
        QString file = QString("%1/%2.conf").arg(path, module);
        if (QFile::exists(file))
            return file;
    }

    return "";
}


/************************************************
 Copy the system configuration, if necessary.
 ************************************************/
void copySysConfig(const QString& module, const QString& homeFile)
{
    QFile hf(homeFile);
    if (hf.exists())
        return;

    QString sysFile(getSysPath(module));
    if (sysFile.isEmpty())
        return;

    QFile sf(sysFile);
    if (!sf.copy(homeFile))
    {
        qWarning() << "Cannot copy file from:" << sysFile << "to:" << homeFile;
        Q_ASSERT(0);
    }

//    qDebug() << "Copied file:" << sysFile << "into" << homeFile;
}


/************************************************

 ************************************************/
QString getFileName(const QString& module)
{
    QString homeFile(RAZOR_HOME_CFG + module + ".conf");
    createDir(homeFile);
    copySysConfig(module, homeFile);
    return homeFile;
}


/************************************************

 ************************************************/
RazorSettings::RazorSettings(const QString& module, QObject* parent) :
    QSettings(getFileName(module), QSettings::IniFormat, parent),
    d_ptr(new RazorSettingsPrivate(this))
{
    sync();
}


/************************************************

 ************************************************/
RazorSettings::RazorSettings(const QSettings* parentSettings, const QString& subGroup, QObject* parent):
    QSettings(parentSettings->fileName(), QSettings::IniFormat, parent),
    d_ptr(new RazorSettingsPrivate(this))
{
    beginGroup(subGroup);
}


/************************************************

 ************************************************/
RazorSettings::RazorSettings(const QSettings& parentSettings, const QString& subGroup, QObject* parent):
    QSettings(parentSettings.fileName(), QSettings::IniFormat, parent),
    d_ptr(new RazorSettingsPrivate(this))
{
    beginGroup(subGroup);
}


/************************************************

 ************************************************/
bool RazorSettings::event(QEvent *event)
{
    if (event->type() == QEvent::UpdateRequest)
    {
            emit settigsChanged();
    }

    return QSettings::event(event);
}
