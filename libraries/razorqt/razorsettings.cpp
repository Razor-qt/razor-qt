/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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

#include "razorsettings.h"
#include <qtxdg/xdgicon.h>
#include <QDebug>
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtCore/QMutex>
#include <QtCore/QFileSystemWatcher>


QString razorConfigDir()
{
#ifdef RAZOR_CONFIG_DIR
    return QDir::cleanPath(QDir::homePath() + "/" + RAZOR_CONFIG_DIR) + "/";
#else
    return QDir::cleanPath(QDir::homePath() + "/.razor") + "/";
#endif
}


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
        paths << razorConfigDir();

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

    return QString();
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
    QString homeFile(razorConfigDir() + module + ".conf");
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

RazorSettings::~RazorSettings()
{
    // because in the RazorSettings::RazorSettings(const QString& module, QObject* parent)
    // constructor there is no beginGroup() called...
    if (!group().isEmpty())
        endGroup();

    delete d_ptr;
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
const RazorSettings *RazorSettings::globalSettings()
{
    static QMutex mutex;
    static GlobalRazorSettings *instance = 0;
    if (!instance)
    {
        mutex.lock();

        if (!instance)
            instance = new GlobalRazorSettings();

        mutex.unlock();
    }

    return instance;
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

RazorTheme::~RazorTheme()
{
    delete d_ptr;
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
    return QString();
}


/************************************************

 ************************************************/
QString RazorThemePrivate::loadQss(const QString& qssFile) const
{
    QFile f(qssFile);
    if (! f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Theme: Canot open file for reading:" << qssFile;
        return QString();
    }

    QString qss = f.readAll();
    f.close();

    if (qss.isEmpty())
        return QString();

    // handle relative paths
    QString qssDir = QFileInfo(qssFile).canonicalPath();
    qss.replace(QRegExp("url.[ \\t\\s]*", Qt::CaseInsensitive, QRegExp::RegExp2), "url(" + qssDir + "/");

    return qss;
}


/************************************************

 ************************************************/
QString RazorTheme::desktopBackground(int screen) const
{
    Q_D(const RazorTheme);
    QString wallpapperCfgFileName = findFile(QString("themes/%1/wallpapper.cfg").arg(d->mThemeName));

    if (wallpapperCfgFileName.isEmpty())
        return QString();

    QSettings s(wallpapperCfgFileName, QSettings::IniFormat);
    QString themeDir = QFileInfo(wallpapperCfgFileName).absolutePath();
    // There is something strange... If I remove next line the wallpapers array is not found...
    s.childKeys();
    s.beginReadArray("wallpapers");

    s.setArrayIndex(screen - 1);
    if (s.contains("file"))
        return QString("%1/%2").arg(themeDir, s.value("file").toString());

    s.setArrayIndex(0);
    if (s.contains("file"))
        return QString("%1/%2").arg(themeDir, s.value("file").toString());

    return QString();
}


/************************************************

 ************************************************/
RazorSettingsCache::RazorSettingsCache(QSettings &settings) :
    mSettings(settings)
{
    loadFromSettings();
}


/************************************************

 ************************************************/
RazorSettingsCache::RazorSettingsCache(QSettings *settings) :
    mSettings(*settings)
{
    loadFromSettings();
}


/************************************************

 ************************************************/
void RazorSettingsCache::loadFromSettings()
{
   foreach (QString key, mSettings.allKeys())
   {
       mCache.insert(key, mSettings.value(key));
   }
}


/************************************************

 ************************************************/
void RazorSettingsCache::loadToSettings()
{
    QHash<QString, QVariant>::const_iterator i = mCache.constBegin();

    while(i != mCache.constEnd())
    {
        mSettings.setValue(i.key(), i.value());
        ++i;
    }

    mSettings.sync();
}


/************************************************

 ************************************************/
class GlobalRazorSettingsPrivate
{
public:
    GlobalRazorSettingsPrivate(GlobalRazorSettings *parent):
        mParent(parent)
    {

    }

    GlobalRazorSettings *mParent;
    QFileSystemWatcher mWatcher;
    QString mIconTheme;
    QString mRazorTheme;

};


/************************************************

 ************************************************/
GlobalRazorSettings::GlobalRazorSettings():
    RazorSettings("razor"),
    d_ptr(new GlobalRazorSettingsPrivate(this))
{
    d_ptr->mWatcher.addPath(this->fileName());
    connect(&(d_ptr->mWatcher), SIGNAL(fileChanged(QString)), this, SLOT(fileChanged()));

    if (value("icon_theme").toString().isEmpty())
    {
        QStringList failback;
        failback << "oxygen";
        failback << "Humanity";

        QDir dir("/usr/share/icons/");
        foreach (QString s, failback)
        {
            if (dir.exists(s))
            {
                setValue("icon_theme", s);
                sync();
                break;
            }
        }
    }

    fileChanged();
}

GlobalRazorSettings::~GlobalRazorSettings()
{
    delete d_ptr;
}


/************************************************

 ************************************************/
void GlobalRazorSettings::fileChanged()
{
    Q_D(GlobalRazorSettings);
    sync();


    QString it = value("icon_theme").toString();
    if (d->mIconTheme != it)
    {
        d->mIconTheme = it;
        XdgIcon::setThemeName(it);
        emit iconThemeChanged();
    }


    QString rt = value("razor_theme").toString();
    if (d->mIconTheme != rt)
    {
        d->mRazorTheme = rt;
        emit razorThemeChanged();
    }

    emit settigsChanged();
}
