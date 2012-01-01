/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL3+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
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
#include <stdlib.h>

#include "xdgdesktopfile.h"
#include "xdgmime.h"
#include "xdgicon.h"
#include "xdgdirs.h"

#include <stdlib.h>
#include <QtCore/QSharedData>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QDebug>
#include <QtCore/QHash>
#include <QtCore/QProcess>
#include <QUrl>
#include <QDesktopServices>
#include <unistd.h>


enum Triple {
    Undef,
    True,
    False
};

class XdgDesktopFileData: public QSharedData {
public:
    XdgDesktopFileData();
    bool read(const QString &prefix);
    XdgDesktopFile::Type detectType(XdgDesktopFile *q) const;
    bool startApplicationDetached(const XdgDesktopFile *q, const QStringList& urls) const;
    bool startLinkDetached(const XdgDesktopFile *q) const;

    QString mFileName;
    bool mIsValid;
    mutable bool mValidIsChecked;
    mutable Triple mIsShow;
    QMap<QString, QVariant> mItems;

    XdgDesktopFile::Type mType;
};


/************************************************

 ************************************************/
XdgDesktopFileData::XdgDesktopFileData():
    mIsValid(false),
    mValidIsChecked(false),
    mIsShow(Undef)
{
}


/************************************************

 ************************************************/
bool XdgDesktopFileData::read(const QString &prefix)
{
    QFile file(mFileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QString section;
    QTextStream stream(&file);
    bool prefixExists = false;
    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed();

        // Skip comments ......................
        if (line.startsWith('#'))
            continue;


        // Section ..............................
        if (line.startsWith('[') && line.endsWith(']'))
        {
            section = line.mid(1, line.length()-2);
            if (section == prefix)
                prefixExists = true;

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


    // Not check for empty prefix
    mIsValid = (prefix.isEmpty()) || prefixExists;
    return mIsValid;
}


/************************************************

 ************************************************/
XdgDesktopFile::Type XdgDesktopFileData::detectType(XdgDesktopFile *q) const
{
    QString typeStr = q->value("Type").toString();
    if (typeStr == "Application")
        return XdgDesktopFile::ApplicationType;

    if (typeStr == "Link")
        return XdgDesktopFile::LinkType;

    if (typeStr == "Directory")
        return XdgDesktopFile::DirectoryType;

    if (!q->value("Exec").toString().isEmpty())
        return XdgDesktopFile::ApplicationType;

    return XdgDesktopFile::UnknownType;
}


/************************************************

 ************************************************/
bool XdgDesktopFileData::startApplicationDetached(const XdgDesktopFile *q, const QStringList& urls) const
{
    //qDebug() << "XdgDesktopFilePrivate::startDetached: urls=" << urls;
    QStringList args = q->expandExecString(urls);

    if (args.isEmpty())
        return false;

    if (q->value("Terminal").toBool())
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
bool XdgDesktopFileData::startLinkDetached(const XdgDesktopFile *q) const
{
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
XdgDesktopFile::XdgDesktopFile():
    d(new XdgDesktopFileData)
{
}


/************************************************

 ************************************************/
XdgDesktopFile::XdgDesktopFile(const XdgDesktopFile& other):
    d(other.d)
{
}

/************************************************

 ************************************************/
XdgDesktopFile::XdgDesktopFile(Type type, const QString& name, const QString &value):
    d(new XdgDesktopFileData)
{
    d->mFileName = name + ".desktop";
    d->mType = type;
    setValue("Version", "1.0");
    setValue("Name", name);
    if (type == XdgDesktopFile::ApplicationType)
    {
        setValue("Type", "Application");
        setValue("Exec", value);
    }
    else if (type == XdgDesktopFile::LinkType)
    {
        setValue("Type", "Link");
        setValue("URL", value);
    }
    else if (type == XdgDesktopFile::DirectoryType)
    {
        setValue("Type", "Directory");
    }
    d->mIsValid = check();
}


/************************************************

 ************************************************/
XdgDesktopFile::~XdgDesktopFile()
{
}


/************************************************

 ************************************************/
XdgDesktopFile& XdgDesktopFile::operator=(const XdgDesktopFile& other)
{
    d = other.d;
    return *this;
}


/************************************************

 ************************************************/
bool XdgDesktopFile::load(const QString& fileName)
{
    d->mFileName = fileName;
    d->read(prefix());
    d->mIsValid = d->mIsValid && check();
    d->mType = d->detectType(this);
    return isValid();
}


/************************************************

 ************************************************/
bool XdgDesktopFile::save(QIODevice *device) const
{
    QTextStream stream(device);
    QMap<QString, QVariant>::const_iterator i = d->mItems.constBegin();

    QString section;
    while (i != d->mItems.constEnd())
    {
        QString path = i.key();
        QString sect =  path.section('/',0,0);
        if (sect != section)
        {
            section = sect;
            stream << "[" << section << "]" << endl;
        }
        QString key = path.section('/', 1);
        stream << key << "=" << i.value().toString() << endl;
        ++i;
    }
    return true;
}


/************************************************

 ************************************************/
bool XdgDesktopFile::save(const QString &fileName) const
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    return save(&file);
}


/************************************************

 ************************************************/
QVariant XdgDesktopFile::value(const QString& key, const QVariant& defaultValue) const
{
    QString path = (!prefix().isEmpty()) ? prefix() + "/" + key : key;
    QVariant v = d->mItems.value(path, defaultValue);
    return v.toString().replace("&", "&&");
}


/************************************************

 ************************************************/
void XdgDesktopFile::setValue(const QString &key, const QVariant &value)
{
    QString path = (!prefix().isEmpty()) ? prefix() + "/" + key : key;
    d->mItems[path] = value;
}


/************************************************

 ************************************************/
void XdgDesktopFile::setLocalizedValue(const QString &key, const QVariant &value)
{
    setValue(localizedKey(key), value);
}


/************************************************
 LC_MESSAGES value	Possible keys in order of matching
 lang_COUNTRY@MODIFIER	lang_COUNTRY@MODIFIER, lang_COUNTRY, lang@MODIFIER, lang,
                        default value
 lang_COUNTRY	        lang_COUNTRY, lang, default value
 lang@MODIFIER	        lang@MODIFIER, lang, default value
 lang	                lang, default value
 ************************************************/
QString XdgDesktopFile::localizedKey(const QString& key) const
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
        if (contains(k))
            return k;
    }

    if (!country.isEmpty())
    {
        QString k = QString("%1[%2_%3]").arg(key, lang, country);
        //qDebug() << "\t try " << k  << contains(k);
        if (contains(k))
            return k;
    }

    if (!modifier.isEmpty())
    {
        QString k = QString("%1[%2@%3]").arg(key, lang, modifier);
        //qDebug() << "\t try " << k  << contains(k);
        if (contains(k))
            return k;
    }

    QString k = QString("%1[%2]").arg(key, lang);
    //qDebug() << "\t try " << k  << contains(k);
    if (contains(k))
        return k;


    //qDebug() << "\t try " << key  << contains(key);
    return key;
}

/************************************************

 ************************************************/
QVariant XdgDesktopFile::localizedValue(const QString& key, const QVariant& defaultValue) const
{
    return value(localizedKey(key), defaultValue);
}


/************************************************

 ************************************************/
bool XdgDesktopFile::contains(const QString& key) const
{
    QString path = (!prefix().isEmpty()) ? prefix() + "/" + key : key;
    return d->mItems.contains(path);
}


/************************************************

 ************************************************/
bool XdgDesktopFile::isValid() const
{
    return d->mIsValid;
}


/************************************************

 ************************************************/
QString XdgDesktopFile::fileName() const
{
    return d->mFileName;
}


/************************************************

 ************************************************/
QIcon const XdgDesktopFile::icon(const QIcon& fallback) const
{
    return XdgIcon::fromTheme(value("Icon").toString(), fallback);
}


/************************************************

 ************************************************/
QString const XdgDesktopFile::iconName() const
{
    return value("Icon").toString();
}


/************************************************

 ************************************************/
XdgDesktopFile::Type XdgDesktopFile::type() const
{
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
    switch(d->mType)
    {
    case ApplicationType:
        return d->startApplicationDetached(this, urls);

    case LinkType:
        return d->startLinkDetached(this);

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
void replaceVar(QString &str, const QString &varName, const QString &after)
{
    str.replace(QRegExp(QString("\\$%1(?!\\w)").arg(varName)), after);
    str.replace(QRegExp(QString("\\$\\{%1\\}").arg(varName)), after);
}


/************************************************

 ************************************************/
QString expandEnvVariables(const QString str)
{
    QString scheme = QUrl(str).scheme();

    if (scheme == "http"   || scheme == "https" || scheme == "shttp" ||
        scheme == "ftp"    || scheme == "ftps"  ||
        scheme == "pop"    || scheme == "pops"  ||
        scheme == "imap"   || scheme == "imaps" ||
        scheme == "mailto" ||
        scheme == "nntp"   ||
        scheme == "irc"    ||
        scheme == "telnet" ||
        scheme == "xmpp"   ||
        scheme == "irc"    ||
        scheme == "nfs"
      )
        return str;

    QString res = str;
    res.replace(QRegExp("~(?=$|/)"), getenv("HOME"));

    replaceVar(res, "HOME", getenv("HOME"));
    replaceVar(res, "USER", getenv("USER"));

    replaceVar(res, "XDG_DESKTOP_DIR",   QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));
    replaceVar(res, "XDG_TEMPLATES_DIR", QDesktopServices::storageLocation(QDesktopServices::TempLocation));
    replaceVar(res, "XDG_DOCUMENTS_DIR", QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation));
    replaceVar(res, "XDG_MUSIC_DIR",     QDesktopServices::storageLocation(QDesktopServices::MusicLocation));
    replaceVar(res, "XDG_PICTURES_DIR",  QDesktopServices::storageLocation(QDesktopServices::PicturesLocation));
    replaceVar(res, "XDG_VIDEOS_DIR",    QDesktopServices::storageLocation(QDesktopServices::MoviesLocation));
    replaceVar(res, "XDG_PHOTOS_DIR",    QDesktopServices::storageLocation(QDesktopServices::PicturesLocation));
    replaceVar(res, "XDG_MOVIES_DIR",    QDesktopServices::storageLocation(QDesktopServices::MoviesLocation));

    return res;
}


/************************************************

 ************************************************/
QStringList expandEnvVariables(const QStringList strs)
{
    QStringList res;
    foreach(QString s, strs)
        res << expandEnvVariables(s);

    return res;
}


/************************************************

 ************************************************/
QStringList XdgDesktopFile::expandExecString(const QStringList& urls) const
{
    if (d->mType != ApplicationType)
        return QStringList();

    QStringList result;
    QStringList tokens = parseCombinedArgString(value("Exec").toString());
    foreach (QString token, tokens)
    {
        // ----------------------------------------------------------
        // A single file name, even if multiple files are selected.
        if (token == "%f")
        {
            if (!urls.isEmpty())
                result << expandEnvVariables(urls.at(0));
            continue;
        }

        // ----------------------------------------------------------
        // A list of files. Use for apps that can open several local files at once.
        // Each file is passed as a separate argument to the executable program.
        if (token == "%F")
        {
            result << expandEnvVariables(urls);
            continue;
        }

        // ----------------------------------------------------------
        // A single URL. Local files may either be passed as file: URLs or as file path.
        if (token == "%u")
        {
            if (!urls.isEmpty())
            {
                QUrl url;
                url.setUrl(expandEnvVariables(urls.at(0)));
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
                QUrl url(expandEnvVariables(s));
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
        result << expandEnvVariables(token);
    }

    return result;
}



/************************************************
 Check if the program is actually installed.
 ************************************************/
bool checkTryExec(const QString& progName)
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
bool XdgDesktopFile::isShow(const QString& environment) const
{
    if (d->mIsShow != Undef)
        return d->mIsShow == True;

    d->mIsShow = False;
    // Means "this application exists, but don't display it in the menus".
    if (value("NoDisplay").toBool())
        return false;

    // The file is inapplicable to the current environment
    if (!isApplicable(true, environment))
        return false;

    d->mIsShow = True;
    return true;
}


/************************************************

 ************************************************/
bool XdgDesktopFile::isApplicable(bool excludeHidden, const QString& environment) const
{
    // Hidden should have been called Deleted. It means the user deleted
    // (at his level) something that was present
    if (excludeHidden && value("Hidden").toBool())
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
    if (!s.isEmpty() && ! checkTryExec(s))
        return false;

    return true;
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
        return QString();

   QString url;

   url = value("URL").toString();
   if (!url.isEmpty())
   return url;

    // WTF? What standard describes it?
    url = expandDynamicUrl(value("URL[$e]").toString());
    if (!url.isEmpty())
        return url;

    return QString();
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
        QString cn = d.canonicalFilePath();
        if (dirName != cn)
        {
            QString f = findDesktopFile(cn, desktopName);
            if (!f.isEmpty())
                return f;
        }
    }

    return QString();
}


/************************************************

 ************************************************/
QString findDesktopFile(const QString& desktopName)
{
    QStringList dataDirs = XdgDirs::dataDirs();
    dataDirs.prepend(XdgDirs::dataHome(false));

    foreach (QString dirName, dataDirs)
    {
        QString f = findDesktopFile(dirName + "/applications", desktopName);
        if (!f.isEmpty())
            return f;
    }

    return QString();
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
        XdgDesktopFile* desktopFile = new XdgDesktopFile();
        desktopFile->load(fileName);
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
            desktopFile = new XdgDesktopFile();
            desktopFile->load(filePath);
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
        dataDirs.prepend(XdgDirs::dataHome(false));

        foreach (QString dirName, dataDirs)
            loadMimeCacheDir(dirName + "/applications", &cache);
    }
    // ..........................................


    if (cache.contains(mimeType))
        return cache.value(mimeType);
    else
        return 0;
}







//class XdgDesktopFilePrivate {
//public:
//    XdgDesktopFilePrivate(XdgDesktopFile* parent);

//    QString prefix() const { return mPrefix; }
//    void setPrefix(const QString& prefix);

//    QString fileName() const { return mFileName; }
//    void setFileName(const QString& fileName) { mFileName = fileName; }

//    XdgDesktopFilePrivate& operator=(const XdgDesktopFilePrivate& other);
//    bool read();

//    bool isValid() const { return mIsValid; }

//    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;
//    QVariant localizedValue(const QString& key, const QVariant& defaultValue = QVariant()) const;

//    bool contains(const QString& key) const;
//    bool isShow(const QString& environment) const;



//    QIcon const icon(const QIcon& fallback = QIcon()) const;

//    XdgDesktopFile::Type mType;
//protected:
//    bool checkTryExec(const QString& progName) const;
//    QStringList expandExecString(const QStringList& urls) const;
//    QString expandEnvVariables(const QString str) const;
//    QStringList expandEnvVariables(const QStringList strs) const;

//private:

//
//    XdgDesktopFile* const q_ptr;
//    Q_DECLARE_PUBLIC(XdgDesktopFile);
//    QString mPrefix;
//    QString mFileName;
//    bool    mIsValid;
//    QMap<QString, QVariant> mItems;
//    mutable IsShow   mIsShow;

//};
























///************************************************

// ************************************************/
//void XdgDesktopFilePrivate::setPrefix(const QString& prefix)
//{
//    mPrefix = prefix;
//    if (prefix.endsWith('/'))
//        mPrefix.truncate(mPrefix.count() - 1);
//}


///************************************************

// ************************************************/
//XdgDesktopFilePrivate& XdgDesktopFilePrivate::operator=(const XdgDesktopFilePrivate& other)
//{
//    mFileName = other.mFileName;
//    mPrefix = other.mPrefix;
//    mIsValid = other.mIsValid;
//    mItems = other.mItems; // This copy map values
//    mIsShow = other.mIsShow;
//    mType = other.mType;
//    return *this;
//}



















///************************************************

// ************************************************/
//QStringList XdgDesktopFilePrivate::expandExecString(const QStringList& urls) const
//{
//    QStringList result;
//    QStringList tokens = parseCombinedArgString(value("Exec").toString());
//    foreach (QString token, tokens)
//    {
//        // ----------------------------------------------------------
//        // A single file name, even if multiple files are selected.
//        if (token == "%f")
//        {
//            if (!urls.isEmpty())
//                result << expandEnvVariables(urls.at(0));
//            continue;
//        }

//        // ----------------------------------------------------------
//        // A list of files. Use for apps that can open several local files at once.
//        // Each file is passed as a separate argument to the executable program.
//        if (token == "%F")
//        {
//            result << expandEnvVariables(urls);
//            continue;
//        }

//        // ----------------------------------------------------------
//        // A single URL. Local files may either be passed as file: URLs or as file path.
//        if (token == "%u")
//        {
//            if (!urls.isEmpty())
//            {
//                QUrl url;
//                url.setUrl(expandEnvVariables(urls.at(0)));
//                result << ((!url.toLocalFile().isEmpty()) ? url.toLocalFile() : url.toEncoded());
//            }
//            continue;
//        }

//        // ----------------------------------------------------------
//        // A list of URLs. Each URL is passed as a separate argument to the executable
//        // program. Local files may either be passed as file: URLs or as file path.
//        if (token == "%U")
//        {
//            foreach (QString s, urls)
//            {
//                QUrl url(expandEnvVariables(s));
//                result << ((!url.toLocalFile().isEmpty()) ? url.toLocalFile() : url.toEncoded());
//            }
//            continue;
//        }

//        // ----------------------------------------------------------
//        // The Icon key of the desktop entry expanded as two arguments, first --icon
//        // and then the value of the Icon key. Should not expand to any arguments if
//        // the Icon key is empty or missing.
//        if (token == "%i")
//        {
//            QString icon = value("Icon").toString();
//            if (!icon.isEmpty())
//                result << "-icon" << icon.replace('%', "%%");
//            continue;
//        }


//        // ----------------------------------------------------------
//        // The translated name of the application as listed in the appropriate Name key
//        // in the desktop entry.
//        if (token == "%c")
//        {
//            result << localizedValue("Name").toString().replace('%', "%%");
//            continue;
//        }

//        // ----------------------------------------------------------
//        // The location of the desktop file as either a URI (if for example gotten from
//        // the vfolder system) or a local filename or empty if no location is known.
//        if (token == "%k")
//        {
//            result << fileName().replace('%', "%%");
//            break;
//        }

//        // ----------------------------------------------------------
//        // Deprecated.
//        // Deprecated field codes should be removed from the command line and ignored.
//        if (token == "%d" || token == "%D" ||
//            token == "%n" || token == "%N" ||
//            token == "%v" || token == "%m"
//            )
//        {
//            continue;
//        }

//        // ----------------------------------------------------------
//        result << expandEnvVariables(token);
//    }

//    return result;
//}






