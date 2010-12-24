/********************************************************************
  Copyright: 2010 Alexander Sokoloff <sokoloff.a@gmail.ru>

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

/************************************************

 ************************************************/
XdgDesktopFile::XdgDesktopFile(QObject *parent) :
QObject(parent)
{
    mPrefix = "Desktop Entry/";
    mIsValid = false;
    mIsShow = -1;
}


/************************************************

 ************************************************/
XdgDesktopFile::XdgDesktopFile(const QString & fileName, QObject *parent) :
QObject(parent)
{
    mFileName = fileName;
    mPrefix = "Desktop Entry/";
    mIsValid = read();
    mIsShow = -1;
}


/************************************************

 ************************************************/
XdgDesktopFile::XdgDesktopFile(const XdgDesktopFile & other, QObject *parent) :
QObject(parent)
{
    this->operator=(other);
}


/************************************************

 ************************************************/
XdgDesktopFile& XdgDesktopFile::operator=(const XdgDesktopFile& other)
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
XdgDesktopFile::~XdgDesktopFile()
{
}


/************************************************

 ************************************************/
bool XdgDesktopFile::read()
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

    return valid;
}


/************************************************

 ************************************************/
QVariant XdgDesktopFile::value(const QString& key, const QVariant& defaultValue) const
{
    return mItems.value(mPrefix + key, defaultValue);
}


/************************************************
 LC_MESSAGES value	Possible keys in order of matching
 lang_COUNTRY@MODIFIER	lang_COUNTRY@MODIFIER, lang_COUNTRY, lang@MODIFIER, lang,
                        default value
 lang_COUNTRY	        lang_COUNTRY, lang, default value
 lang@MODIFIER	        lang@MODIFIER, lang, default value
 lang	                lang, default value
 ************************************************/
QVariant XdgDesktopFile::localizedValue(const QString& key, const QVariant& defaultValue) const
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
bool XdgDesktopFile::contains(const QString& key) const
{
    return mItems.contains(mPrefix + key);
}


/************************************************

 ************************************************/
bool XdgDesktopFile::isShow(const QString& environment) const
{
    if (mIsShow > -1)
        return mIsShow > 0;

    mIsShow = 0;

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

    mIsShow = 1;
    return true;
}


/************************************************
 Check if the program is actually installed.
 ************************************************/
bool XdgDesktopFile::checkTryExec(const QString& progName) const
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
 Starts the program with the arguments in a new process, and detaches from it.
 ExecString argument consists of an executable program optionally followed by
 one or more arguments.
 Returns true on success; otherwise returns false.
 See http://standards.freedesktop.org/desktop-entry-spec/latest/ar01s06.html
 ************************************************/
bool XdgDesktopFile::execute(const QString& execString)
{
    /**********************************
     TODO: Sokoloff
     This is a very dirty implementation, I promise to rewrite it :)
     **********************************/
    /*
    %f	 A single file name
    %F	 A list of files.
    %u	 A single URL.
    %U	 A list of URLs.
    %c	 The translated name.
    %k	 The location of the desktop file.

    %d	 Deprecated.
    %D	 Deprecated.
    %n	 Deprecated.
    %N	 Deprecated.
    %v	 Deprecated.
    %m	 Deprecated.
    */

    QString cmd = execString;

    cmd.remove("%i");
    cmd.remove("%d");
    cmd.remove("%U");
    cmd.remove("%u");
    cmd.remove("%F");
    cmd.remove("%f");

    return QProcess::startDetached(cmd);
}


/************************************************

 ************************************************/
XdgDesktopFile* XdgDesktopFileCache::getFile(const QString& fileName)
{
    static QHash<QString, XdgDesktopFile*> mDesktopFiles;
    if (mDesktopFiles.contains(fileName))
         return mDesktopFiles.value(fileName);

     XdgDesktopFile* desktopFile = new XdgDesktopFile(fileName);
//     if (!desktopFile->isValid())
//     {
//         delete desktopFile;
//         return 0;
//     }

     mDesktopFiles.insert(fileName, desktopFile);
     return desktopFile;
}


