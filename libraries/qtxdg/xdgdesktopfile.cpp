/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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
#include <QList>
#include <QtAlgorithms>
#include <unistd.h>
#include <QSettings>
#include <QTextStream>
#include <QFile>
/************************************************

 ************************************************/
QString &doEscape(QString& str, const QHash<QChar,QChar> &repl)
{
    // First we replace slash.
    str.replace('\\', "\\\\");

    QHashIterator<QChar,QChar> i(repl);
    while (i.hasNext()) {
        i.next();
        if (i.key() != '\\')
            str.replace(i.key(), QString("\\\\%1").arg(i.value()));
    }

    return str;
}

/************************************************
 The escape sequences \s, \n, \t, \r, and \\ are supported for values
 of type string and localestring, meaning ASCII space, newline, tab,
 carriage return, and backslash, respectively.
 ************************************************/
QString &escape(QString& str)
{
    QHash<QChar,QChar> repl;
    repl.insert('\n',  'n');
    repl.insert('\t',  't');
    repl.insert('\r',  'r');

    return doEscape(str, repl);
}


/************************************************
 Quoting must be done by enclosing the argument between double quotes and
 escaping the
    double quote character,
    backtick character ("`"),
    dollar sign ("$") and
    backslash character ("\")
by preceding it with an additional backslash character.
Implementations must undo quoting before expanding field codes and before
passing the argument to the executable program.

Note that the general escape rule for values of type string states that the
backslash character can be escaped as ("\\") as well and that this escape
rule is applied before the quoting rule. As such, to unambiguously represent a
literal backslash character in a quoted argument in a desktop entry file
requires the use of four successive backslash characters ("\\\\").
Likewise, a literal dollar sign in a quoted argument in a desktop entry file
is unambiguously represented with ("\\$").
 ************************************************/
QString &escapeExec(QString& str)
{
    QHash<QChar,QChar> repl;
    // The parseCombinedArgString() splits the string by the space symbols,
    // we temporarily replace them on the special characters.
    // Replacement will reverse after the splitting.
    repl.insert('"', '"');    // double quote,
    repl.insert('\'', '\'');  // single quote ("'"),
    repl.insert('\\', '\\');  // backslash character ("\"),
    repl.insert('$', '$');    // dollar sign ("$"),

    return doEscape(str, repl);
}



/************************************************

 ************************************************/
QString &doUnEscape(QString& str, const QHash<QChar,QChar> &repl)
{
    int n = 0;
    while (1)
    {
        n=str.indexOf("\\", n);
        if (n < 0 || n > str.length() - 2)
            break;

        if (repl.contains(str.at(n+1)))
        {
            str.replace(n, 2, repl.value(str.at(n+1)));
        }

        n++;
    }

    return str;
}


/************************************************
 The escape sequences \s, \n, \t, \r, and \\ are supported for values
 of type string and localestring, meaning ASCII space, newline, tab,
 carriage return, and backslash, respectively.
 ************************************************/
QString &unEscape(QString& str)
{
    QHash<QChar,QChar> repl;
    repl.insert('\\', '\\');
    repl.insert('s',  ' ');
    repl.insert('n',  '\n');
    repl.insert('t',  '\t');
    repl.insert('r',  '\r');

    return doUnEscape(str, repl);
}



/************************************************
 Quoting must be done by enclosing the argument between double quotes and
 escaping the
    double quote character,
    backtick character ("`"),
    dollar sign ("$") and
    backslash character ("\")
by preceding it with an additional backslash character.
Implementations must undo quoting before expanding field codes and before
passing the argument to the executable program.

Reserved characters are
    space (" "),
    tab,
    newline,
    double quote,
    single quote ("'"),
    backslash character ("\"),
    greater-than sign (">"),
    less-than sign ("<"),
    tilde ("~"),
    vertical bar ("|"),
    ampersand ("&"),
    semicolon (";"),
    dollar sign ("$"),
    asterisk ("*"),
    question mark ("?"),
    hash mark ("#"),
    parenthesis ("(") and (")")
    backtick character ("`").

Note that the general escape rule for values of type string states that the
backslash character can be escaped as ("\\") as well and that this escape
rule is applied before the quoting rule. As such, to unambiguously represent a
literal backslash character in a quoted argument in a desktop entry file
requires the use of four successive backslash characters ("\\\\").
Likewise, a literal dollar sign in a quoted argument in a desktop entry file
is unambiguously represented with ("\\$").
 ************************************************/
QString &unEscapeExec(QString& str)
{
    unEscape(str);
    QHash<QChar,QChar> repl;
    // The parseCombinedArgString() splits the string by the space symbols,
    // we temporarily replace them on the special characters.
    // Replacement will reverse after the splitting.
    repl.insert(' ',  01);    // space
    repl.insert('\t', 02);    // tab
    repl.insert('\n', 03);    // newline,

    repl.insert('"', '"');    // double quote,
    repl.insert('\'', '\'');  // single quote ("'"),
    repl.insert('\\', '\\');  // backslash character ("\"),
    repl.insert('>', '>');    // greater-than sign (">"),
    repl.insert('<', '<');    // less-than sign ("<"),
    repl.insert('~', '~');    // tilde ("~"),
    repl.insert('|', '|');    // vertical bar ("|"),
    repl.insert('&', '&');    // ampersand ("&"),
    repl.insert(';', ';');    // semicolon (";"),
    repl.insert('$', '$');    // dollar sign ("$"),
    repl.insert('*', '*');    // asterisk ("*"),
    repl.insert('?', '?');    // question mark ("?"),
    repl.insert('#', '#');    // hash mark ("#"),
    repl.insert('(', '(');    // parenthesis ("(")
    repl.insert(')', ')');    // parenthesis (")")
    repl.insert('`', '`');    // backtick character ("`").

    return doUnEscape(str, repl);
}

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
        qWarning() << "XdgDesktopFileData::startLinkDetached: url is empty.";
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
bool XdgDesktopFile::operator==(const XdgDesktopFile &other) const
{
    return d->mItems == other.d->mItems;
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
    QVariant res = d->mItems.value(path, defaultValue);
    if (res.type() == QVariant::String)
    {
        QString s = res.toString();
        return unEscape(s);
    }

    return res;
}


/************************************************

 ************************************************/
void XdgDesktopFile::setValue(const QString &key, const QVariant &value)
{
    QString path = (!prefix().isEmpty()) ? prefix() + "/" + key : key;
    if (value.type() == QVariant::String)
    {

        QString s=value.toString();
        if (key.toUpper() == "EXEC")
            escapeExec(s);
        else
            escape(s);

        d->mItems[path] = QVariant(s);

        if (key.toUpper() == "TYPE")
            d->mType = d->detectType(this);
    }
    else
    {
        d->mItems[path] = value;
    }
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
void XdgDesktopFile::removeEntry(const QString& key)
{
    QString path = (!prefix().isEmpty()) ? prefix() + "/" + key : key;
    d->mItems.remove(path);
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
    QIcon result = XdgIcon::fromTheme(value("Icon").toString(), fallback);

    if (result.isNull() && type() == ApplicationType) {
        result = XdgIcon::fromTheme("application-x-executable.png");
        // TODO Maybe defaults for other desktopfile types as well..
    }

    return result;
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

    QString execStr = value("Exec").toString();
    unEscapeExec(execStr);
    QStringList tokens = parseCombinedArgString(execStr);

    foreach (QString token, tokens)
    {
        // The parseCombinedArgString() splits the string by the space symbols,
        // we temporarily replaced them on the special characters.
        // Now we reverse it.
        token.replace(01, ' ');
        token.replace(02, '\t');
        token.replace(03, '\n');

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
        QStringList s = value("OnlyShowIn").toString().split(';');
        if (!s.contains(environment))
            return false;
    }

    // NotShowIn .........
    if (contains("NotShowIn"))
    {
        QStringList s = value("NotShowIn").toString().split(';');
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
    foreach(QString line, QProcess::systemEnvironment())
    {
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
    if (instance().m_fileCache.contains(fileName))
    {
        return instance().m_fileCache.value(fileName);
    }
    
    if (fileName.startsWith(QDir::separator()))
    {
        // Absolute path ........................
        //qDebug() << "XdgDesktopFileCache: add new file" << fileName;
        XdgDesktopFile* desktopFile = load(fileName);
        instance().m_fileCache.insert(fileName, desktopFile);
        return desktopFile;
    }
    else
    {
        // Search desktop file ..................
        QString filePath = findDesktopFile(fileName);
        //qDebug() << "Sokoloff XdgDesktopFileCache::getFile found fileName" << fileName << filePath;
        XdgDesktopFile* desktopFile;

        if (!instance().m_fileCache.contains(filePath))
        {
            desktopFile = load(filePath);
            instance().m_fileCache.insert(filePath, desktopFile);
        }
        else
            desktopFile = instance().m_fileCache.value(filePath);

        instance().m_fileCache.insert(fileName, desktopFile);
        return desktopFile;
    }
}

QList<XdgDesktopFile*> XdgDesktopFileCache::getAllFiles()
{
    return instance().m_fileCache.values();
}



XdgDesktopFileCache & XdgDesktopFileCache::instance()
{
    static XdgDesktopFileCache cache;
    if (!cache.m_IsInitialized)
    {
       cache.initialize(); 
       cache.m_IsInitialized = true;
    }

    return cache;
}



/*! 
 * Handles files with a syntax similar to desktopfiles as QSettings files.
 * The differences between ini-files and desktopfiles are:
 * desktopfiles uses '#' as comment marker, and ';' as list-separator.
 * Every key/value must be inside a section (i.e. there is no 'General' pseudo-section)
 */
bool readDesktopFile(QIODevice & device, QSettings::SettingsMap & map)
{
    QString section;
    QTextStream stream(&device);
    
    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed();

        // Skip comments and empty lines
        if (line.startsWith('#') || line.isEmpty())
            continue;

        // Section ..............................
        if (line.startsWith('[') && line.endsWith(']'))
        {
            section = line.mid(1, line.length()-2);
            continue;
        }

        QString key = line.section('=', 0, 0).trimmed();
        QString value = line.section('=', 1).trimmed();

        if (key.isEmpty())
            continue;
   
        if (section.isEmpty())
        {
            qWarning() << "key=value outside section";
            return false;
        }

        key.prepend("/");
        key.prepend(section);

        if (value.contains(";")) 
        {
            map.insert(key, value.split(";"));
        }
        else
        {
            map.insert(key, value);
        }

    }
    
    return true;
}

/*! See readDesktopFile
 */
bool writeDesktopFile(QIODevice & device, const QSettings::SettingsMap & map)
{
    QTextStream stream(&device);
    QString section; 

    foreach (QString key, map.keys())
    {
        if (! map.value(key).canConvert<QString>())
        {
            return false;
        }

        QString thisSection = key.section("/", 0, 0);
        if (thisSection.isEmpty())
        {
            qWarning() << "No section defined";
            return false;
        }

        if (thisSection != section)
        {
            stream << "[" << thisSection << "]" << "\n";
            section = thisSection;
        }

        QString remainingKey = key.section("/", 1, -1);
        
        if (remainingKey.isEmpty())
        {
            qWarning() << "Only one level in key..." ;
            return false;
        }

        stream << remainingKey << "=" << map.value(key).toString() << "\n";

    }
        
    return true;
}


/************************************************

 ************************************************/
void XdgDesktopFileCache::initialize(const QString& dirName)
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
            initialize(f.absoluteFilePath());
            continue;
        }


        XdgDesktopFile* df = load(f.absoluteFilePath());
        if (!df)
            continue;

        if (! m_fileCache.contains(f.absoluteFilePath()))
        {
            m_fileCache.insert(f.absoluteFilePath(), df);
        }
        
        QStringList mimes = df->value("MimeType").toString().split(';', QString::SkipEmptyParts); 

        foreach (QString mime, mimes)
        {
            int pref = df->value("InitialPreference", 0).toInt();
            // We move the desktopFile forward in the list for this mime, so that 
            // no desktopfile in front of it have a lower initialPreference.
            int position = m_defaultAppsCache[mime].length();
            while (position > 0 && m_defaultAppsCache[mime][position - 1]->value("InitialPreference, 0").toInt() < pref)
            {
                position--;
            }
            m_defaultAppsCache[mime].insert(position, df);
        }
    }

}

XdgDesktopFile* XdgDesktopFileCache::load(const QString& fileName)
{
    XdgDesktopFile* desktopFile = new XdgDesktopFile();
    desktopFile->load(fileName);
    return desktopFile;
}

void loadMimeCacheDir(const QString& dirName, QHash<QString, QList<XdgDesktopFile*> > & cache)
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

        foreach (QString mime, mimes)
        {
            int pref = df->value("InitialPreference", 0).toInt();
            // We move the desktopFile forward in the list for this mime, so that 
            // no desktopfile in front of it have a lower initialPreference.
            int position = cache[mime].length();
            while (position > 0 && cache[mime][position - 1]->value("InitialPreference, 0").toInt() < pref)
            {
                position--;
            }
            cache[mime].insert(position, df);
        }
    }
}

QSettings::Format XdgDesktopFileCache::desktopFileSettingsFormat()
{
    static QSettings::Format format = QSettings::InvalidFormat;

    if (format == QSettings::InvalidFormat)
    {
        format = QSettings::registerFormat("*.list", readDesktopFile, writeDesktopFile);
        qDebug() << "registerFormat returned:" << format;
    }

    return format;
}


XdgDesktopFileCache::XdgDesktopFileCache() :
    m_IsInitialized(false),
        m_defaultAppsCache(),
        m_fileCache()
{
}

XdgDesktopFileCache::~XdgDesktopFileCache()
{
}



void XdgDesktopFileCache::initialize()
{
    QStringList dataDirs = XdgDirs::dataDirs();
    dataDirs.prepend(XdgDirs::dataHome(false));

    foreach (const QString dirname, dataDirs) 
    {
        initialize(dirname + "/applications");
//        loadMimeCacheDir(dirname + "/applications", m_defaultAppsCache);
    }
}

QList<XdgDesktopFile*>  XdgDesktopFileCache::getApps(const QString& mimetype)
{
    return instance().m_defaultAppsCache.value(mimetype);
}


/************************************************

 ************************************************/
XdgDesktopFile* XdgDesktopFileCache::getDefaultApp(const QString& mimetype)
{
    // First, we look in ~/.local/share/applications/defaults.list, /usr/local/share/applications/defaults.list and
    // /usr/share/applications/defaults.list (in that order) for a default.
    QStringList dataDirs = XdgDirs::dataDirs();
    dataDirs.prepend(XdgDirs::dataHome(false));
    foreach(const QString dataDir, dataDirs)
    {
        QString defaultsListPath = dataDir + "/applications/defaults.list";
        if (QFileInfo(defaultsListPath).exists())
        {
            QSettings defaults(defaultsListPath, desktopFileSettingsFormat());
            

            defaults.beginGroup("Default Applications");
            if (defaults.contains(mimetype))
            {
                QVariant value = defaults.value(mimetype);
                if (value.canConvert<QStringList>()) // A single string can also convert to a stringlist
                {
                    foreach (const QString desktopFileName, value.toStringList())
                    {
                        XdgDesktopFile* desktopFile = XdgDesktopFileCache::getFile(desktopFileName);
                        if (desktopFile->isValid())
                        {
                            return desktopFile;
                        }
                        else 
                        {
                            qWarning() << desktopFileName << "not a valid desktopfile";
                        }
                    }
                }
            }
            defaults.endGroup();
        }
    }

    // If we havent found anything up to here, we look for a desktopfile that declares
    // the ability to handle the given mimetype. See getApps.
    QList<XdgDesktopFile*> apps = getApps(mimetype);
    XdgDesktopFile* desktopFile = apps.isEmpty() ? 0 : apps[0];
    return desktopFile;
}

