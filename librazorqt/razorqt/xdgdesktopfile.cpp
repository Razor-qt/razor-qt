/********************************************************************
  Copyright: 2010-2011 Alexander Sokoloff <sokoloff.a@gmail.ru>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

 *********************************************************************
  See: http://standards.freedesktop.org/desktop-entry-spec

*********************************************************************/

#include "xdgdesktopfile.h"
#include <stdlib.h>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QHash>
#include <QProcess>
// For XdgDesktopFilePrivate
#include <QStack>
#include <QUrl>
#include "xdgicon.h"
#include "xdgenv.h"

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

    bool startDetached(const QStringList& urls) const;

    QIcon const icon(const QIcon& fallback = QIcon()) const;

protected:
    bool checkTryExec(const QString& progName) const;
    QString expandExecString(const QStringList& urls) const;
    int expandMacro(const QString &str, int pos, const QStringList& urls, QStringList &ret) const;

private:
    enum IsShow {
        ShowUndefined,
        ShowEnabled,
        ShowDisabled
    };
    enum Quoting
    {
        noquote,
        singlequote,
        doublequote,
        dollarquote,
        paren,
        subst,
        group,
        math
    };

    typedef struct
    {
        Quoting current;
        bool dquote;
    } State;

    typedef struct
    {
        QString str;
        int pos;
    } Save;

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
inline static bool isSpecial( QChar cUnicode )
{
    static const uchar iqm[] = {
        0xff, 0xff, 0xff, 0xff, 0xdf, 0x07, 0x00, 0xd8,
        0x00, 0x00, 0x00, 0x38, 0x01, 0x00, 0x00, 0x78
    }; // 0-32 \'"$`<>|;&(){}*?#!~[]

    uint c = cUnicode.unicode ();
    return (c < sizeof(iqm) * 8) && (iqm[c / 8] & (1 << (c & 7)));
}


/************************************************

 ************************************************/
QString quoteArg( const QString &arg )
{
    if (!arg.length())
        return QString::fromLatin1("''");
    for (int i = 0; i < arg.length(); i++)
        if (isSpecial( arg.unicode()[i] )) {
            QChar q( QLatin1Char('\'') );
            return QString( arg ).replace( q, QLatin1String("'\\''") ).prepend( q ).append( q );
        }
    return arg;
}


/************************************************

 ************************************************/
QString joinArgs( const QStringList &args )
{
    QString ret;
    for (QStringList::ConstIterator it = args.begin(); it != args.end(); ++it) {
        if (!ret.isEmpty())
            ret.append(QLatin1Char(' '));
        ret.append(quoteArg(*it));
    }
    return ret;
}


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
QString XdgDesktopFile::expandExecString(const QStringList& urls) const
{
    Q_D(const XdgDesktopFile);
    return d->expandExecString(urls);
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
    return d->startDetached(urls);
}


/************************************************
 This is an overloaded function.
 ************************************************/
bool XdgDesktopFile::startDetached(const QString& url) const
{
    Q_D(const XdgDesktopFile);
    if (url.isEmpty())
        return d->startDetached(QStringList());
    else
        return d->startDetached(QStringList(url));
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

    mIsValid = valid;
    return valid;
}


/************************************************

 ************************************************/
QVariant XdgDesktopFilePrivate::value(const QString& key, const QVariant& defaultValue) const
{
    //qDebug() << "XdgDesktopFilePrivate::value mPrefix + key" << mPrefix + key;
    return mItems.value(mPrefix + key, defaultValue);
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
QString XdgDesktopFilePrivate::expandExecString(const QStringList& urls) const
{
    int len;
    int pos2;
    ushort ec = '%';
    State state = { noquote, false };
    QStack<State> sstack;
    QStack<Save> ostack;
    QStringList rst;
    QString rsts;

    int pos = 0;
    QString str = value("Exec").toString();

    while (pos < str.length()) {
        ushort cc = str.unicode()[pos].unicode();
        if (cc != ec)
            goto nohit;

        if (!(len = expandMacro( str, pos, urls, rst )))
            goto nohit;

        if (len < 0)
        {
            pos -= len;
            continue;
        }

        if (state.dquote)
        {
            rsts = rst.join( QLatin1String(" ") );
            rsts.replace( QRegExp(QLatin1String("([$`\"\\\\])")), QLatin1String("\\\\1") );
        }
        else if (state.current == dollarquote)
        {
            rsts = rst.join( QLatin1String(" ") );
            rsts.replace( QRegExp(QLatin1String("(['\\\\])")), QLatin1String("\\\\1") );
        }
        else if (state.current == singlequote)
        {
            rsts = rst.join( QLatin1String(" ") );
            rsts.replace( QLatin1Char('\''), QLatin1String("'\\''") );
        }
        else
        {
            if (rst.isEmpty())
            {
                str.remove( pos, len );
                continue;
            }
            else
            {
                rsts = joinArgs( rst );
            }
        }
        rst.clear();
        str.replace( pos, len, rsts );
        pos += rsts.length();
        continue;

    nohit:
        if (state.current == singlequote)
        {
            if (cc == '\'')
                state = sstack.pop();
        }
        else if (cc == '\\')
        {
            // always swallow the char -> prevent anomalies due to expansion
            pos += 2;
            continue;
        }
        else if (state.current == dollarquote)
        {
            if (cc == '\'')
                state = sstack.pop();
        }
        else if (cc == '$')
        {
            cc = str.unicode()[++pos].unicode();
            if (cc == '(')
            {
                sstack.push( state );
                if (str.unicode()[pos + 1].unicode() == '(')
                {
                    Save sav = { str, pos + 2 };
                    ostack.push( sav );
                    state.current = math;
                    pos += 2;
                    continue;
                }
                else
                {
                    state.current = paren;
                    state.dquote = false;
                }
            }
            else if (cc == '{')
            {
                sstack.push( state );
                state.current = subst;
            }
            else if (!state.dquote)
            {
                if (cc == '\'')
                {
                    sstack.push( state );
                    state.current = dollarquote;
                }
                else if (cc == '"')
                {
                    sstack.push( state );
                    state.current = doublequote;
                    state.dquote = true;
                }
            }
            // always swallow the char -> prevent anomalies due to expansion
        }
        else if (cc == '`')
        {
            str.replace( pos, 1, QLatin1String("$( " )); // add space -> avoid creating $((
            pos2 = pos += 3;
            for (;;)
            {
                if (pos2 >= str.length())
                {
                    pos = pos2;
                    return false;
                }
                cc = str.unicode()[pos2].unicode();
                if (cc == '`')
                    break;
                if (cc == '\\')
                {
                    cc = str.unicode()[++pos2].unicode();
                    if (cc == '$' || cc == '`' || cc == '\\' ||
                        (cc == '"' && state.dquote))
                    {
                        str.remove( pos2 - 1, 1 );
                        continue;
                    }
                }
                pos2++;
            }
            str[pos2] = QLatin1Char(')');
            sstack.push( state );
            state.current = paren;
            state.dquote = false;
            continue;
        }
        else if (state.current == doublequote)
        {
            if (cc == '"')
                state = sstack.pop();
        }
        else if (cc == '\'')
        {
            if (!state.dquote)
            {
                sstack.push( state );
                state.current = singlequote;
            }
        }
        else if (cc == '"')
        {
            if (!state.dquote)
            {
                sstack.push( state );
                state.current = doublequote;
                state.dquote = true;
            }
        }
        else if (state.current == subst)
        {
            if (cc == '}')
                state = sstack.pop();
        }
        else if (cc == ')')
        {
            if (state.current == math)
            {
                if (str.unicode()[pos + 1].unicode() == ')')
                {
                    state = sstack.pop();
                    pos += 2;
                }
                else
                {
                    // false hit: the $(( was a $( ( in fact
                    // ash does not care, but bash does
                    pos = ostack.top().pos;
                    str = ostack.top().str;
                    ostack.pop();
                    state.current = paren;
                    state.dquote = false;
                    sstack.push( state );
                }
                continue;
            }
            else if (state.current == paren)
                state = sstack.pop();
            else
                break;
        }
        else if (cc == '}')
        {
            if (state.current == group)
                state = sstack.pop();
            else
                break;
        }
        else if (cc == '(')
        {
            sstack.push( state );
            state.current = paren;
        }
        else if (cc == '{')
        {
            sstack.push( state );
            state.current = group;
        }
        pos++;
    }

    if (sstack.empty())
        return str;
    else
        return QString();
}


/************************************************

 ************************************************/
int XdgDesktopFilePrivate::expandMacro(const QString &str, int pos, const QStringList& urls, QStringList &ret) const
{
    uint option = str[pos + 1].unicode();
    switch (option)
    {
        // ----------------------------------------------------------
        // A single file name, even if multiple files are selected.
        case 'f':
        {
            if (!urls.isEmpty())
                ret << urls.at(0);
            break;
        }

        // ----------------------------------------------------------
        // A list of files. Use for apps that can open several local files at once.
        // Each file is passed as a separate argument to the executable program.
        case 'F':
        {
            ret << urls;
            break;
        }

        // ----------------------------------------------------------
        // A single URL. Local files may either be passed as file: URLs or as file path.
        case 'u':
        {
            if (urls.isEmpty())
                break;

            QUrl url;
            url.setUrl(urls.at(0));
            ret << ((!url.toLocalFile().isEmpty()) ? url.toLocalFile() : url.toEncoded());
            break;
        }

        // ----------------------------------------------------------
        // A list of URLs. Each URL is passed as a separate argument to the executable
        // program. Local files may either be passed as file: URLs or as file path.
        case 'U':
        {
            foreach (QString s, urls)
            {
                QUrl url(s);
                ret << ((!url.toLocalFile().isEmpty()) ? url.toLocalFile() : url.toEncoded());
            }
            break;
        }

        // ----------------------------------------------------------
        // The Icon key of the desktop entry expanded as two arguments, first --icon
        // and then the value of the Icon key. Should not expand to any arguments if
        // the Icon key is empty or missing.
        case 'i':
        {
            QString icon = value("Icon").toString();
            if (!icon.isEmpty())
                ret << "-icon" << icon.replace('%', "%%");

            break;
        }


        // ----------------------------------------------------------
        // The translated name of the application as listed in the appropriate Name key
        // in the desktop entry.
        case 'c':
        {
            ret << localizedValue("Name").toString().replace('%', "%%");
            break;
        }

        // ----------------------------------------------------------
        // The location of the desktop file as either a URI (if for example gotten from
        // the vfolder system) or a local filename or empty if no location is known.
        case 'k':
        {
            ret << fileName().replace('%', "%%");
            break;
        }

        // ----------------------------------------------------------
        // Deprecated.
        // Deprecated field codes should be removed from the command line and ignored.
        case 'd':
        case 'D':
        case 'n':
        case 'N':
        case 'v':
        case 'm':
            return -2;

        // ----------------------------------------------------------
        case '%':
            ret = QStringList(QLatin1String("%"));
            break;

        // ----------------------------------------------------------
        default:
            return -2; // subst with same and skip
    }

    return 2;
}


/************************************************

 ************************************************/
bool XdgDesktopFilePrivate::startDetached(const QStringList& urls) const
{
    //qDebug() << "XdgDesktopFilePrivate::startDetached: fileName" << fileName();
    qDebug() << "XdgDesktopFilePrivate::startDetached: urls=" << urls;
    QString cmd = expandExecString(urls);
    //qDebug() << "XdgDesktopFilePrivate.startDetached: cmd=" << cmd;
    if (cmd.isEmpty())
        return false;

    if (value("Terminal").toBool())
    {
        QString term = getenv("TERM");
        if (term.isEmpty())
            term = "xterm";

        cmd = QString("%1 -e %2").arg(term, cmd);
    }

    qDebug() << "XdgDesktopFilePrivate.startDetached: run command:" << cmd;

    return QProcess::startDetached(cmd);
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
    QStringList dataDirs = XdgEnv::dataDirs().split(":", QString::SkipEmptyParts);

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
        qDebug() << "Sokoloff XdgDesktopFileCache::getFile found fileName" << fileName << filePath;
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
QIcon const XdgDesktopFilePrivate::icon(const QIcon& fallback) const
{
    return XdgIcon::fromTheme(value("Icon").toString(), fallback);
}
