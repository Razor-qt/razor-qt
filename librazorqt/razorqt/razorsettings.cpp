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
#include <QtCore/QMutex>


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


RazorTheme* RazorTheme::mInstance = 0;

class RazorThemePrivate
{
public:
    RazorThemePrivate(RazorTheme* parent):
        mParent(parent)
    {
    }

    QString loadQss(const QString& qssFile) const;
    QString mThemeName;
    RazorTheme* mParent;
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
  This looks in all the usual paths for file
 ************************************************/
QString findFile(const QString& fileName, bool onlyGlobal = false)
{
    QStringList paths;

    if (!onlyGlobal)
        paths << RAZOR_HOME_CFG;

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
        QString file = QString("%1/%2").arg(path, fileName);
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

    QString sysFile(findFile(QString("%1.conf").arg(module), true));
    if (sysFile.isEmpty())
        return;

    QFile sf(sysFile);
    if (!sf.copy(homeFile))
    {
        qWarning() << "Cannot copy file from:" << sysFile << "to:" << homeFile;
        Q_ASSERT(0);
    }
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


/************************************************

 ************************************************/
RazorTheme::RazorTheme():
    QObject(),
    d_ptr(new RazorThemePrivate(this))
{
    RazorSettings settings("razor");
    d_ptr->mThemeName = settings.value("theme").toString();
}


/************************************************

 ************************************************/
RazorTheme* RazorTheme::instance()
{
    static QMutex mutex;

    if (!mInstance)
    {
        mutex.lock();

        if (!mInstance)
            mInstance = new RazorTheme();

        mutex.unlock();
    }

    return mInstance;
}


/************************************************

 ************************************************/
QString RazorTheme::qss(const QString& module) const
{
    Q_D(const RazorTheme);


    QString path(findFile(QString("themes/%1/%2.qss").arg(d->mThemeName, module)));
    if (!path.isEmpty())
        return d->loadQss(path);

    qWarning() << "QSS file cannot be found in any location:" << QString("%1/%2.qss").arg(d->mThemeName, module);
    return "";
}


/************************************************

 ************************************************/
QString RazorThemePrivate::loadQss(const QString& qssFile) const
{
    QFile f(qssFile);
    if (! f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Theme: Canot open file for reading:" << qssFile;
        return "";
    }

    QString qss = f.readAll();
    f.close();

    if (qss.isEmpty())
        return "";

    // handle relative paths
    QString qssDir = QFileInfo(qssFile).canonicalPath();
    qss.replace(QRegExp("url.[ \\t\\s]*", Qt::CaseInsensitive, QRegExp::RegExp2), "url(" + qssDir + "/");

    return qss;
}


QString RazorTheme::desktopBackground(int screen) const
{
    Q_D(const RazorTheme);
    QString indexFileName = findFile(QString("themes/%1/index.theme").arg(d->mThemeName));

    if (indexFileName.isEmpty())
        return "";

    QSettings s(indexFileName, QSettings::IniFormat);
    // There is something strange... If I remove next line the wallpapers array is not found...
    s.childKeys();
    s.beginReadArray("wallpapers");

    s.setArrayIndex(screen);
    if (s.contains("file"))
        return s.value("file").toString();

    s.setArrayIndex(0);
    if (s.contains("file"))
        return s.value("file").toString();

    return QString();
}
