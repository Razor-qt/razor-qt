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
#include <qtxdg/xdgdirs.h>
#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtCore/QMutex>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QSharedData>

class RazorSettingsPrivate
{
public:
    RazorSettingsPrivate(RazorSettings* parent):
        mParent(parent)
    {
    }

    QString localizedKey(const QString& key) const;

    QFileSystemWatcher mWatcher;

private:
    RazorSettings* mParent;
};


RazorTheme* RazorTheme::mInstance = 0;

class RazorThemeData: public QSharedData {
public:
    RazorThemeData(): mValid(false) {}
    QString loadQss(const QString& qssFile) const;
    QString findTheme(const QString &themeName);

    QString mName;
    QString mPath;
    QString mPreviewImg;
    bool mValid;

};


class GlobalRazorSettingsPrivate
{
public:
    GlobalRazorSettingsPrivate(GlobalRazorSettings *parent):
        mParent(parent),
        mThemeUpdated(0ull)
    {

    }

    GlobalRazorSettings *mParent;
    QString mIconTheme;
    QString mRazorTheme;
    qlonglong mThemeUpdated;

};


/************************************************

 ************************************************/
RazorSettings::RazorSettings(const QString& module, QObject* parent) :
    QSettings("razor", module, parent),
    d_ptr(new RazorSettingsPrivate(this))
{
    // HACK: we need to ensure that the user (~/.config/razor/<module>.conf)
    //       exists to have functional mWatcher
    if (!contains("__userfile__"))
    {
        setValue("__userfile__", true);
        sync();
    }
    d_ptr->mWatcher.addPath(this->fileName());
    connect(&(d_ptr->mWatcher), SIGNAL(fileChanged(QString)), this, SLOT(fileChanged()));
}


/************************************************

 ************************************************/
RazorSettings::RazorSettings(const QString &fileName, QSettings::Format format, QObject *parent):
    QSettings(fileName, format, parent),
    d_ptr(new RazorSettingsPrivate(this))
{
    // HACK: we need to ensure that the user (~/.config/razor/<module>.conf)
    //       exists to have functional mWatcher
    if (!contains("__userfile__"))
    {
        setValue("__userfile__", true);
        sync();
    }
    d_ptr->mWatcher.addPath(this->fileName());
    connect(&(d_ptr->mWatcher), SIGNAL(fileChanged(QString)), this, SLOT(fileChanged()));
}


/************************************************

 ************************************************/
RazorSettings::RazorSettings(const QSettings* parentSettings, const QString& subGroup, QObject* parent):
    QSettings(parentSettings->organizationName(), parentSettings->applicationName(), parent),
    d_ptr(new RazorSettingsPrivate(this))
{
    beginGroup(subGroup);
}


/************************************************

 ************************************************/
RazorSettings::RazorSettings(const QSettings& parentSettings, const QString& subGroup, QObject* parent):
    QSettings(parentSettings.organizationName(), parentSettings.applicationName(), parent),
    d_ptr(new RazorSettingsPrivate(this))
{
    beginGroup(subGroup);
}


/************************************************

 ************************************************/
RazorSettings::~RazorSettings()
{
    // because in the RazorSettings::RazorSettings(const QString& module, QObject* parent)
    // constructor there is no beginGroup() called...
    if (!group().isEmpty())
        endGroup();

    delete d_ptr;
}

bool RazorSettings::event(QEvent *event)
{
    if (event->type() == QEvent::UpdateRequest)
    {
        emit settingsChanged();
    }

    return QSettings::event(event);
}

void RazorSettings::fileChanged()
{
//    Q_D(RazorSettings);
    sync();
    emit settingsChanged();
}


/************************************************

 ************************************************/
const GlobalRazorSettings *RazorSettings::globalSettings()
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
 LC_MESSAGES value	Possible keys in order of matching
 lang_COUNTRY@MODIFIER	lang_COUNTRY@MODIFIER, lang_COUNTRY, lang@MODIFIER, lang,
                        default value
 lang_COUNTRY	        lang_COUNTRY, lang, default value
 lang@MODIFIER	        lang@MODIFIER, lang, default value
 lang	                lang, default value
 ************************************************/
QString RazorSettingsPrivate::localizedKey(const QString& key) const
{

    QString lang = getenv("LC_MESSAGES");

    if (lang.isEmpty())
        lang = getenv("LC_ALL");

    if (lang.isEmpty())
         lang = getenv("LANG");


    QString modifier = lang.section('@', 1);
    if (!modifier.isEmpty())
        lang.truncate(lang.length() - modifier.length() - 1);

    QString encoding = lang.section('.', 1);
    if (!encoding.isEmpty())
        lang.truncate(lang.length() - encoding.length() - 1);


    QString country = lang.section('_', 1);
    if (!country.isEmpty())
        lang.truncate(lang.length() - country.length() - 1);



    //qDebug() << "LC_MESSAGES: " << getenv("LC_MESSAGES");
    //qDebug() << "Lang:" << lang;
    //qDebug() << "Country:" << country;
    //qDebug() << "Encoding:" << encoding;
    //qDebug() << "Modifier:" << modifier;

    if (!modifier.isEmpty() && !country.isEmpty())
    {
        QString k = QString("%1[%2_%3@%4]").arg(key, lang, country, modifier);
        //qDebug() << "\t try " << k << mParent->contains(k);
        if (mParent->contains(k))
            return k;
    }

    if (!country.isEmpty())
    {
        QString k = QString("%1[%2_%3]").arg(key, lang, country);
        //qDebug() << "\t try " << k  << mParent->contains(k);
        if (mParent->contains(k))
            return k;
    }

    if (!modifier.isEmpty())
    {
        QString k = QString("%1[%2@%3]").arg(key, lang, modifier);
        //qDebug() << "\t try " << k  << mParent->contains(k);
        if (mParent->contains(k))
            return k;
    }

    QString k = QString("%1[%2]").arg(key, lang);
    //qDebug() << "\t try " << k  << mParent->contains(k);
    if (mParent->contains(k))
        return k;


    //qDebug() << "\t try " << key  << mParent->contains(key);
    return key;
}


/************************************************

 ************************************************/
QVariant RazorSettings::localizedValue(const QString& key, const QVariant& defaultValue) const
{
    Q_D(const RazorSettings);
    return value(d->localizedKey(key), defaultValue);
}


/************************************************

 ************************************************/
void RazorSettings::setLocalizedValue(const QString &key, const QVariant &value)
{
    Q_D(const RazorSettings);
    setValue(d->localizedKey(key), value);
}


/************************************************

 ************************************************/
RazorTheme::RazorTheme():
    d(new RazorThemeData)
{
}


/************************************************

 ************************************************/
RazorTheme::RazorTheme(const QString &path):
    d(new RazorThemeData)
{
    if (path.isEmpty())
        return;

    QFileInfo fi(path);
    if (fi.isAbsolute())
    {
        d->mPath = path;
        d->mName = fi.fileName();
        d->mValid = fi.isDir();
    }
    else
    {
        d->mName = path;
        d->mPath = d->findTheme(path);
        d->mValid = !(d->mPath.isEmpty());
    }

    if (QDir(path).exists("preview.png"))
        d->mPreviewImg = path + "/preview.png";
}


/************************************************

 ************************************************/
QString RazorThemeData::findTheme(const QString &themeName)
{
    if (themeName.isEmpty())
        return "";

    QStringList paths;
    paths << XdgDirs::dataHome(false);
    paths << XdgDirs::dataDirs();

    foreach(QString path, paths)
    {
        QDir dir(QString("%1/razor/themes/%2").arg(path, themeName));
        if (dir.isReadable())
            return dir.absolutePath();
    }

    return QString();
}


/************************************************

 ************************************************/
RazorTheme::RazorTheme(const RazorTheme &other):
    d(other.d)
{
}


/************************************************

 ************************************************/
RazorTheme::~RazorTheme()
{
}


/************************************************

 ************************************************/
RazorTheme& RazorTheme::operator=(const RazorTheme &other)
{
    d = other.d;
    return *this;
}


/************************************************

 ************************************************/
bool RazorTheme::isValid() const
{
    return d->mValid;
}


/************************************************

 ************************************************/
QString RazorTheme::name() const
{
    return d->mName;
}

/************************************************

 ************************************************/
QString RazorTheme::path() const
{
    return d->mPath;
}


/************************************************

 ************************************************/
QString RazorTheme::previewImage() const
{
    return d->mPreviewImg;
}


/************************************************

 ************************************************/
QString RazorTheme::qss(const QString& module) const
{
    QString path = QString("%1/%2.qss").arg(d->mPath, module);

    QString styleSheet;
    if (!path.isEmpty())
        styleSheet = d->loadQss(path);
    else
        qWarning() << QString("QSS file %1 cannot be found").arg(path);

    // Single/double click ...........................
    RazorSettings s("desktop");
    bool singleClick = s.value("icon-launch-mode", "singleclick").toString() == "singleclick";
    styleSheet += QString("QAbstractItemView {activate-on-singleclick : %1; }").arg(singleClick ? 1 : 0);

    return styleSheet;
}


/************************************************

 ************************************************/
QString RazorThemeData::loadQss(const QString& qssFile) const
{
    QFile f(qssFile);
    if (! f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Theme: Cannot open file for reading:" << qssFile;
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
    QString wallpaperCfgFileName = QString("%1/wallpaper.cfg").arg(d->mPath);

    if (wallpaperCfgFileName.isEmpty())
        return QString();

    QSettings s(wallpaperCfgFileName, QSettings::IniFormat);
    QString themeDir = QFileInfo(wallpaperCfgFileName).absolutePath();
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
const RazorTheme &RazorTheme::currentTheme()
{
    static RazorTheme theme;
    QString name = RazorSettings::globalSettings()->value("theme").toString();
    if (theme.name() != name)
    {
        theme = RazorTheme(name);
    }
    return theme;
}


/************************************************

 ************************************************/
QList<RazorTheme> RazorTheme::allThemes()
{
    QList<RazorTheme> ret;
    QSet<QString> processed;

    QStringList paths;
    paths << XdgDirs::dataHome(false);
    paths << XdgDirs::dataDirs();

    foreach(QString path, paths)
    {
        QDir dir(QString("%1/razor/themes").arg(path));
        QFileInfoList dirs = dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);

        foreach(QFileInfo dir, dirs)
        {
            if (!processed.contains(dir.fileName()) &&
                 QDir(dir.absoluteFilePath()).exists("razor-panel.qss"))
            {
                processed << dir.fileName();
                ret << RazorTheme(dir.absoluteFilePath());
            }

        }
    }

    return ret;
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
GlobalRazorSettings::GlobalRazorSettings():
    RazorSettings("razor"),
    d_ptr(new GlobalRazorSettingsPrivate(this))
{
    if (value("icon_theme").toString().isEmpty())
    {
        QStringList failback;
        failback << "oxygen";
        failback << "Tango";
        failback << "Prudence-icon";
        failback << "Humanity";
        failback << "elementary";
        failback << "gnome";


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

    QString rt = value("theme").toString();
    qlonglong themeUpdated = value("__theme_updated__").toLongLong();
    if ((d->mRazorTheme != rt) || (d->mThemeUpdated != themeUpdated))
    {
        d->mRazorTheme = rt;
        emit razorThemeChanged();
    }

    emit settingsChanged();
}

