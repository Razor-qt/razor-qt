/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
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



/*********************************************************************
  See: http://standards.freedesktop.org/desktop-entry-spec

*********************************************************************/

#include "xdgdesktopfile.h"
#include "xdgmime.h"
#include "xdgicon.h"
#include "xdgdirs.h"

#include <stdlib.h>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QDebug>
#include <QtCore/QHash>
#include <QtCore/QProcess>
#include <QUrl>
#include <QDesktopServices>
#include <unistd.h>

class XdgDesktopFilePrivate {
public:
    XdgDesktopFilePrivate(XdgDesktopFile* parent);

    QString prefix() const { return mPrefix; }
    void setPrefix(const QString& prefix);

    QString fileName() const { return mFileName; }
    void setFileName(const QString& fileName) { mFileName = fileName; }

    XdgDesktopFilePrivate& operator=(const XdgDesktopFilePrivate& other);
    bool read();

    bool isValid() const { return mIsValid; }

    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;
    QVariant localizedValue(const QString& key, const QVariant& defaultValue = QVariant()) const;

    bool contains(const QString& key) const;
    bool isShow(const QString& environment) const;

    bool startApplicationDetached(const QStringList& urls) const;
    bool startLinkDetached() const;

    QIcon const icon(const QIcon& fallback = QIcon()) const;

    XdgDesktopFile::Type mType;
protected:
    bool checkTryExec(const QString& progName) const;
    QStringList expandExecString(const QStringList& urls) const;


private:
    enum IsShow {
        ShowUndefined,
        ShowEnabled,
        ShowDisabled
    };

    XdgDesktopFile::Type detectType();
    XdgDesktopFile* const q_ptr;
    Q_DECLARE_PUBLIC(XdgDesktopFile);
    QString mPrefix;
    QString mFileName;
    bool    mIsValid;
    QMap<QString, QVariant> mItems;
    mutable IsShow   mIsShow;

};


/************************************************

 ************************************************/
XdgDesktopFile::XdgDesktopFile(QObject *parent, const QString& prefix) :
    QObject(parent),
    d_ptr(new XdgDesktopFilePrivate(this))
{
    Q_D(XdgDesktopFile);
    d->setPrefix(prefix);
}


/************************************************

 ************************************************/
XdgDesktopFile::XdgDesktopFile(const QString & fileName, QObject *parent, const QString& prefix) :
    QObject(parent),
    d_ptr(new XdgDesktopFilePrivate(this))
{
    Q_D(XdgDesktopFile);
    d->setPrefix(prefix);
    d->setFileName(fileName);
    d->read();
}


/************************************************

 ************************************************/
XdgDesktopFile::XdgDesktopFile(const XdgDesktopFile& other, QObject *parent) :
    QObject(parent),
    d_ptr(new XdgDesktopFilePrivate(this))
{
    this->operator=(other);
}


/************************************************

 ************************************************/
XdgDesktopFile& XdgDesktopFile::operator=(const XdgDesktopFile& other)
{
    Q_D(XdgDesktopFile);
    d->operator =(*(other.d_func()));
    return *this;
}


/************************************************

 ************************************************/
XdgDesktopFile::~XdgDesktopFile()
{
    Q_D(XdgDesktopFile);
    delete d;
}


/************************************************

 ************************************************/
bool XdgDesktopFile::isValid() const
{
    Q_D(const XdgDesktopFile);
    return d->isValid();
}

/************************************************

 ************************************************/
QString XdgDesktopFile::fileName() const
{
    Q_D(const XdgDesktopFile);
    return d->fileName();
}


/************************************************

 ************************************************/
QVariant XdgDesktopFile::value(const QString& key, const QVariant& defaultValue) const
{
    Q_D(const XdgDesktopFile);
    return d->value(key, defaultValue);
}


/************************************************

 ************************************************/
QVariant XdgDesktopFile::localizedValue(const QString& key, const QVariant& defaultValue) const
{
    Q_D(const XdgDesktopFile);
    return d->localizedValue(key, defaultValue);
}


/************************************************

 ************************************************/
bool XdgDesktopFile::contains(const QString& key) const
{
    Q_D(const XdgDesktopFile);
    return d->contains(key);
}


/************************************************

 ************************************************/
bool XdgDesktopFile::isShow(const QString& environment) const
{
    Q_D(const XdgDesktopFile);
    return d->isShow(environment);
}


/************************************************

 ************************************************/
QStringList XdgDesktopFile::expandExecString(const QStringList& urls) const
{
    Q_D(const XdgDesktopFile);
    if (d->mType == ApplicationType)
        return d->expandExecString(urls);
    else
        return QStringList();
}


/************************************************

 ************************************************/
QString expandDynamicUrl(QString url)
{
    for (int i=0; environ[i]; i++)
    {
        QString line(environ[i]);
        QString name = line.section("=", 0, 0);
        QString val =  line.section("=", 1);
        url.replace(QString("$%1").arg(name), val);
        url.replace(QString("${%1}").arg(name), val);
    }

    return url;
}


/************************************************

 ************************************************/
QString XdgDesktopFile::url() const
{
    if (type() != LinkType)
        return "";

   QString url;

   url = value("URL").toString();
   if (!url.isEmpty())
   return url;

    // WTF? What standard describes it?
    url = expandDynamicUrl(value("URL[$e]").toString());
    if (!url.isEmpty())
        return url;

    return "";
}


/************************************************

 ************************************************/
XdgDesktopFile::Type XdgDesktopFilePrivate::detectType()
{
    QString typeStr = value("Type").toString();
    if (typeStr == "Application")
        return XdgDesktopFile::ApplicationType;

    if (typeStr == "Link")
        return XdgDesktopFile::LinkType;

    if (typeStr == "Directory")
        return XdgDesktopFile::DirectoryType;

    return XdgDesktopFile::UnknownType;
}


/************************************************

 ************************************************/
XdgDesktopFile::Type XdgDesktopFile::type() const
{
    Q_D(const XdgDesktopFile);
    return d->mType;
}


/************************************************
 Starts the program defined in this desktop file in a new process, and detaches
 from it. Returns true on success; otherwise returns false. If the calling process
 exits, the detached process will continue to live.

 Urls - the list of URLs or files to open, can be empty (app launched without
  argument)
 If the function is successful then *pid is set to the process identifier of the
 started process.
 ************************************************/
bool XdgDesktopFile::startDetached(const QStringList& urls) const
{
    Q_D(const XdgDesktopFile);
    switch(d->mType)
    {
    case ApplicationType:
        return d->startApplicationDetached(urls);

    case LinkType:
        return d->startLinkDetached();

    default:
        return false;
    }
}


/************************************************
 This is an overloaded function.
 ************************************************/
bool XdgDesktopFile::startDetached(const QString& url) const
{
    if (url.isEmpty())
        return startDetached(QStringList());
    else
        return startDetached(QStringList(url));
}


/************************************************
  Returns an icon specified in this file.
 ************************************************/
QIcon const XdgDesktopFile::icon(const QIcon& fallback) const
{
    Q_D(const XdgDesktopFile);
    return d->icon(fallback);
}


/************************************************

 ************************************************/
XdgDesktopFilePrivate::XdgDesktopFilePrivate(XdgDesktopFile* parent):
    q_ptr(parent)
{
    mIsValid = false;
    mPrefix = "";
    mFileName = "";
    mType = XdgDesktopFile::UnknownType;
    mIsShow = XdgDesktopFilePrivate::ShowUndefined;
}


/************************************************

 ************************************************/
void XdgDesktopFilePrivate::setPrefix(const QString& prefix)
{
    mPrefix = prefix + (prefix.endsWith('/')? "" : "/");
}


/************************************************

 ************************************************/
XdgDesktopFilePrivate& XdgDesktopFilePrivate::operator=(const XdgDesktopFilePrivate& other)
{
    mFileName = other.mFileName;
    mPrefix = other.mPrefix;
    mIsValid = other.mIsValid;
    mItems = other.mItems; // This copy map values
    mIsShow = other.mIsShow;
    mType = other.mType;
    return *this;
}


/************************************************

 ************************************************/
bool XdgDesktopFilePrivate::read()
{
    QFile file(mFileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QString section;
    QTextStream stream(&file);
    bool valid = false;
    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed();

        // Skip comments ......................
        if (line.startsWith('#'))
            continue;


        // Section ..............................
        if (line.startsWith('[') && line.endsWith(']'))
        {
            section = line.mid(1, line.length()-2);
            if (section == "Desktop Entry")
                valid = true;

            continue;
        }

        QString key = line.section('=', 0, 0).trimmed();
        QString value = line.section('=', 1).trimmed();

        if (key.isEmpty())
            continue;

        // Remove quotes ........................
        if ((value.startsWith('"') && value.endsWith('"')) ||
            (value.startsWith('\'') && value.endsWith('\'')))
            value = value.mid(1, value.length()-2);

        mItems[section + "/" + key] = QVariant(value);
    }

    mType = detectType();
    mIsValid = valid;
    return valid;
}


/************************************************

 ************************************************/
QVariant XdgDesktopFilePrivate::value(const QString& key, const QVariant& defaultValue) const
{
    //qDebug() << "XdgDesktopFilePrivate::value mPrefix + key" << mPrefix + key;
    QVariant v = mItems.value(mPrefix + key, defaultValue);
    return v.toString().replace("&", "&&");
}


///************************************************
// LC_MESSAGES value	Possible keys in order of matching
// lang_COUNTRY@MODIFIER	lang_COUNTRY@MODIFIER, lang_COUNTRY, lang@MODIFIER, lang,
//                        default value
// lang_COUNTRY	        lang_COUNTRY, lang, default value
// lang@MODIFIER	        lang@MODIFIER, lang, default value
// lang	                lang, default value
// ************************************************/
QVariant XdgDesktopFilePrivate::localizedValue(const QString& key, const QVariant& defaultValue) const
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
        //qDebug() << "\t try " << k << contains(k);
        if (contains(k)) return value(k, defaultValue);
    }

    if (!country.isEmpty())
    {
        QString k = QString("%1[%2_%3]").arg(key, lang, country);
        //qDebug() << "\t try " << k  << contains(k);
        if (contains(k)) return value(k, defaultValue);
    }

    if (!modifier.isEmpty())
    {
        QString k = QString("%1[%2@%3]").arg(key, lang, modifier);
        //qDebug() << "\t try " << k  << contains(k);
        if (contains(k)) return value(k, defaultValue);
    }

    QString k = QString("%1[%2]").arg(key, lang);
    //qDebug() << "\t try " << k  << contains(k);
    if (contains(k)) return value(k, defaultValue);


    //qDebug() << "\t try " << key  << contains(key);
    return value(key, defaultValue);
}



/************************************************

 ************************************************/
bool XdgDesktopFilePrivate::contains(const QString& key) const
{
    return mItems.contains(mPrefix + key);
}


/************************************************

 ************************************************/
bool XdgDesktopFilePrivate::isShow(const QString& environment) const
{
    if (mIsShow != XdgDesktopFilePrivate::ShowUndefined)
        return mIsShow == XdgDesktopFilePrivate::ShowEnabled;

    mIsShow = XdgDesktopFilePrivate::ShowDisabled;
    // Means "this application exists, but don't display it in the menus".
    if (value("NoDisplay").toBool())
        return false;

    // Hidden should have been called Deleted. It means the user deleted
    // (at his level) something that was present
    if (value("Hidden").toBool())
        return false;

    // A list of strings identifying the environments that should display/not
    // display a given desktop entry.
    // OnlyShowIn ........
    if (contains("OnlyShowIn"))
    {
        QString s = ";" + value("OnlyShowIn").toString() + ";";
        if (!s.contains(environment))
            return false;
    }

    // NotShowIn .........
    if (contains("NotShowIn"))
    {
        QString s = ";" + value("NotShowIn").toString() + ";";
        if (s.contains(environment))
            return false;
    }

    // actually installed. If not, entry may not show in menus, etc.
    QString s = value("TryExec").toString();
    if (!s.isEmpty() && ! this->checkTryExec(s))
        return false;

    mIsShow = XdgDesktopFilePrivate::ShowEnabled;
    return true;
}


/************************************************
 Check if the program is actually installed.
 ************************************************/
bool XdgDesktopFilePrivate::checkTryExec(const QString& progName) const
{
    if (progName.startsWith(QDir::separator()))
        return QFileInfo(progName).isExecutable();

    QStringList dirs = QString(getenv("PATH")).split(":");

    foreach (QString dir, dirs)
    {
        if (QFileInfo(QDir(dir), progName).isExecutable())
            return true;
    }

    return false;
}


/************************************************

 ************************************************/
static QStringList parseCombinedArgString(const QString &program)
{
    QStringList args;
    QString tmp;
    int quoteCount = 0;
    bool inQuote = false;

    // handle quoting. tokens can be surrounded by double quotes
    // "hello world". three consecutive double quotes represent
    // the quote character itself.
    for (int i = 0; i < program.size(); ++i) {
        if (program.at(i) == QLatin1Char('"')) {
            ++quoteCount;
            if (quoteCount == 3) {
                // third consecutive quote
                quoteCount = 0;
                tmp += program.at(i);
            }
            continue;
        }
        if (quoteCount) {
            if (quoteCount == 1)
                inQuote = !inQuote;
            quoteCount = 0;
        }
        if (!inQuote && program.at(i).isSpace()) {
            if (!tmp.isEmpty()) {
                args += tmp;
                tmp.clear();
            }
        } else {
            tmp += program.at(i);
        }
    }
    if (!tmp.isEmpty())
        args += tmp;

    return args;
}


/************************************************

 ************************************************/
QStringList XdgDesktopFilePrivate::expandExecString(const QStringList& urls) const
{
    QStringList result;
    QStringList tokens = parseCombinedArgString(value("Exec").toString());
    foreach (QString token, tokens)
    {
        // ----------------------------------------------------------
        // A single file name, even if multiple files are selected.
        if (token == "%f")
        {
            if (!urls.isEmpty())
                result << urls.at(0);
            continue;
        }

        // ----------------------------------------------------------
        // A list of files. Use for apps that can open several local files at once.
        // Each file is passed as a separate argument to the executable program.
        if (token == "%F")
        {
            result << urls;
            continue;
        }

        // ----------------------------------------------------------
        // A single URL. Local files may either be passed as file: URLs or as file path.
        if (token == "%u")
        {
            if (!urls.isEmpty())
            {
                QUrl url;
                url.setUrl(urls.at(0));
                result << ((!url.toLocalFile().isEmpty()) ? url.toLocalFile() : url.toEncoded());
            }
            continue;
        }

        // ----------------------------------------------------------
        // A list of URLs. Each URL is passed as a separate argument to the executable
        // program. Local files may either be passed as file: URLs or as file path.
        if (token == "%U")
        {
            foreach (QString s, urls)
            {
                QUrl url(s);
                result << ((!url.toLocalFile().isEmpty()) ? url.toLocalFile() : url.toEncoded());
            }
            continue;
        }

        // ----------------------------------------------------------
        // The Icon key of the desktop entry expanded as two arguments, first --icon
        // and then the value of the Icon key. Should not expand to any arguments if
        // the Icon key is empty or missing.
        if (token == "%i")
        {
            QString icon = value("Icon").toString();
            if (!icon.isEmpty())
                result << "-icon" << icon.replace('%', "%%");
            continue;
        }


        // ----------------------------------------------------------
        // The translated name of the application as listed in the appropriate Name key
        // in the desktop entry.
        if (token == "%c")
        {
            result << localizedValue("Name").toString().replace('%', "%%");
            continue;
        }

        // ----------------------------------------------------------
        // The location of the desktop file as either a URI (if for example gotten from
        // the vfolder system) or a local filename or empty if no location is known.
        if (token == "%k")
        {
            result << fileName().replace('%', "%%");
            break;
        }

        // ----------------------------------------------------------
        // Deprecated.
        // Deprecated field codes should be removed from the command line and ignored.
        if (token == "%d" || token == "%D" ||
            token == "%n" || token == "%N" ||
            token == "%v" || token == "%m"
            )
        {
            continue;
        }

        // ----------------------------------------------------------
        result << token;
    }

    return result;
}

/************************************************

 ************************************************/
bool XdgDesktopFilePrivate::startApplicationDetached(const QStringList& urls) const
{
    //qDebug() << "XdgDesktopFilePrivate::startDetached: urls=" << urls;
    QStringList args = expandExecString(urls);

    if (args.isEmpty())
        return false;

    if (value("Terminal").toBool())
    {
        QString term = getenv("TERM");
        if (term.isEmpty())
            term = "xterm";

        args.prepend("-e");
        args.prepend(term);
    }

    //qDebug() << "XdgDesktopFilePrivate.startDetached: run command:" << args;
    QString cmd = args.takeFirst();
    return QProcess::startDetached(cmd, args);
}


/************************************************

 ************************************************/
bool XdgDesktopFilePrivate::startLinkDetached() const
{
    Q_Q(const XdgDesktopFile);
    QString url = q->url();

    if (url.isEmpty())
    {
        qWarning() << "XdgDesktopFilePrivate::startLinkDetached: url is empty.";
        return false;
    }

    QString scheme = QUrl(url).scheme();

    if (scheme.isEmpty() || scheme.toUpper() == "FILE")
    {
        // Local file
        QFileInfo fi(url);
        XdgMimeInfo mimeInfo(fi);

        XdgDesktopFile* desktopFile = XdgDesktopFileCache::getDefaultApp(mimeInfo.mimeType());
        if (desktopFile)
            return desktopFile->startDetached(url);
    }
    else
    {
        // Internet URL
        return QDesktopServices::openUrl(QUrl::fromEncoded(url.toLocal8Bit()));
    }

    return false;
}


/************************************************

 ************************************************/
QString findDesktopFile(const QString& dirName, const QString& desktopName)
{
    QDir dir(dirName);
    QFileInfo fi(dir, desktopName);

    if (fi.exists())
        return fi.canonicalFilePath();

    // Working recursively ............
    QFileInfoList dirs = dir.entryInfoList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (QFileInfo d, dirs)
    {
        QString f = findDesktopFile(d.canonicalFilePath(), desktopName);
        if (!f.isEmpty())
            return f;
    }

    return "";
}


/************************************************

 ************************************************/
QString findDesktopFile(const QString& desktopName)
{
    QStringList dataDirs = XdgDirs::dataDirs();

    foreach (QString dirName, dataDirs)
    {
        QString f = findDesktopFile(dirName + "/applications", desktopName);
        if (!f.isEmpty())
            return f;
    }

    return "";
}


/************************************************

 ************************************************/
XdgDesktopFile* XdgDesktopFileCache::getFile(const QString& fileName)
{
    static QHash<QString, XdgDesktopFile*> mDesktopFiles;
    if (mDesktopFiles.contains(fileName))
         return mDesktopFiles.value(fileName);


    if (fileName.startsWith(QDir::separator()))
    {
        // Absolute path ........................
        //qDebug() << "XdgDesktopFileCache: add new file" << fileName;
        XdgDesktopFile* desktopFile = new XdgDesktopFile(fileName);
        mDesktopFiles.insert(fileName, desktopFile);
        return desktopFile;
    }
    else
    {
        // Search desktop file ..................
        QString filePath = findDesktopFile(fileName);
        //qDebug() << "Sokoloff XdgDesktopFileCache::getFile found fileName" << fileName << filePath;
        XdgDesktopFile* desktopFile;

        if (!mDesktopFiles.contains(filePath))
        {
            desktopFile = new XdgDesktopFile(filePath);
            mDesktopFiles.insert(filePath, desktopFile);
        }
        else
            desktopFile = mDesktopFiles.value(filePath);


        mDesktopFiles.insert(fileName, desktopFile);
        return desktopFile;
    }
}



/************************************************

 ************************************************/
void loadMimeCacheDir(const QString& dirName, QHash<QString, XdgDesktopFile*>* cache)
{
    QDir dir(dirName);
    // Directories have the type "application/x-directory", but in the desktop file
    // are shown as "inode/directory". To handle these cases, we use this hash.
    QHash<QString, QString> specials;
    specials.insert("inode/directory", "application/x-directory");


    // Working recursively ............
    QFileInfoList files = dir.entryInfoList(QStringList(), QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (QFileInfo f, files)
    {
        if (f.isDir())
        {
            loadMimeCacheDir(f.absoluteFilePath(), cache);
            continue;
        }


        XdgDesktopFile* df = XdgDesktopFileCache::getFile(f.absoluteFilePath());
        if (!df)
            continue;

        QStringList mimes = df->value("MimeType").toString().split(';', QString::SkipEmptyParts);
        int newPref= df->value("InitialPreference", 0).toInt();

        foreach (QString m, mimes)
        {
            // If the association doesn't exist or its priority is less.
            if (!cache->contains(m) ||
                cache->value(m)->value("InitialPreference", 0).toInt() < newPref)
            {
                cache->insert(m, df);
                if (specials.contains(m))
                    cache->insert(specials.value(m), df);
            }
        }
    }

}


/************************************************

 ************************************************/
XdgDesktopFile* XdgDesktopFileCache::getDefaultApp(const QString& mimeType)
{
    static QHash<QString, XdgDesktopFile*> cache;
    // Initialize the cache .....................
    if (cache.isEmpty())
    {
        QStringList dataDirs = XdgDirs::dataDirs();
        foreach (QString dirName, dataDirs)
            loadMimeCacheDir(dirName + "/applications", &cache);
    }
    // ..........................................


    if (cache.contains(mimeType))
        return cache.value(mimeType);
    else
        return 0;
}


/************************************************

 ************************************************/
QIcon const XdgDesktopFilePrivate::icon(const QIcon& fallback) const
{
    return XdgIcon::fromTheme(value("Icon").toString(), fallback);
}
