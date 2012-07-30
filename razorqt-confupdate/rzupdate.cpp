/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This code based on kconf_update utility.
 *   Copyright (c) 2001 Waldo Bastian <bastian@kde.org>
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

#include "rzupdate.h"
#include <qtxdg/xdgdirs.h>

#include <QtCore/QDebug>
#include <QtCore/QDate>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtCore/QTextCodec>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QTemporaryFile>

#define UPDATE_INFO_GROUP   "__Version__"
#define UPDATE_INFO_KEY     UPDATE_INFO_GROUP "/update_info"

RzUpdate::RzUpdate():
    m_config(0),
    m_oldConfig(0),
    m_newConfig(0),
    m_file(0)

{
    m_debug = false;
}


void RzUpdate::run(const QStringList &_files)
{
    bool updateAll = false;
    m_config = new QSettings("razor", "razor-confupdate");

    QStringList updateFiles;

    m_bUseConfigInfo = false;
    if (! m_argCheckFile.isEmpty())
    {
        m_bUseConfigInfo = true;

        QFileInfo file(m_argCheckFile);
        if (! file.exists())
        {
            qWarning() << QString("File '%1' not found.").arg(m_argCheckFile);
            log() << "File '" << m_argCheckFile << "' passed on command line not found" << endl;
            return;
        }
        updateFiles.append(file.absoluteFilePath());
    }
    else if (_files.count())
    {
        for (int i = 0; i < _files.count(); i++)
        {
            QFileInfo file(_files.at(i));
            if (! file.exists())
            {
                qWarning() << QString("File '%1' not found.").arg(_files.at(i));
                log() << "File '" << _files.at(i) << "' passed on command line not found" << endl;
                return;
            }

            updateFiles.append(file.absoluteFilePath());
        }
    }
    else
    {

        if (m_config->value("autoUpdateDisabled", false).toBool())
            return;
        updateFiles = findUpdateFiles(true);
        updateAll = true;
    }

    for (QStringList::ConstIterator it = updateFiles.constBegin();
            it != updateFiles.constEnd();
            ++it)
    {
        updateFile(*it);
    }

    if (updateAll && !m_config->value("updateInfoAdded", false).toBool())
    {
        m_config->setValue("updateInfoAdded", true);
        updateFiles = findUpdateFiles(false);

        for (QStringList::ConstIterator it = updateFiles.constBegin();
                it != updateFiles.constEnd();
                ++it)
        {
            checkFile(*it);
        }
        updateFiles.clear();
    }
}

RzUpdate::~RzUpdate()
{
    delete m_config;
    delete m_file;
}

QTextStream & operator<<(QTextStream & stream, const QStringList & lst)
{
    stream << lst.join(", ");
    return stream;
}

QTextStream & RzUpdate::log() const
{
    static QTextStream stream;
    static QFile file;

    if (!stream.device())
    {
        QDir dir(QString("%1/razor").arg(XdgDirs::dataHome()));
        if (!dir.exists())
            dir.mkpath(".");

        file.setFileName(dir.absoluteFilePath("razor-confupdate.log"));
        file.open(QIODevice::WriteOnly | QIODevice::Append);

        if (!file.isOpen())
            file.open(stdout, QIODevice::WriteOnly);

        stream.setDevice(&file);
    }

    stream << QDateTime::currentDateTime().toString(Qt::ISODate) << " ";

    return stream;
}

QTextStream & RzUpdate::logFileError() const
{
    return log() << m_currentFilename << ':' << m_lineCount << ":'" << m_line << "': ";
}

QStringList RzUpdate::updDirs()
{
    QStringList dirs;
    dirs << XdgDirs::dataHome(false) + "/razor/razor-confupdate";

    foreach (QString dir, XdgDirs::dataDirs())
        dirs << dir + "/razor/razor-confupdate";

    return dirs;
}

QStringList RzUpdate::findUpdateFiles(bool dirtyOnly)
{
    QStringList result;
    QStringList dirs = updDirs();

    foreach (QString dirName, dirs)
    {
        QDir dir(dirName);
        const QFileInfoList list = dir.entryInfoList(QStringList() << "*.upd",
                                                     QDir::Files | QDir::Readable
                                                    );
        foreach (const QFileInfo &file, list )
        {
            if (!dirtyOnly)
            {
                result.append(file.absoluteFilePath());
            }
            else
            {
                QDateTime ctime = QDateTime::fromString(m_config->value(file.fileName() + "/ctime").toString(), Qt::ISODate);
                QDateTime mtime = QDateTime::fromString(m_config->value(file.fileName() + "/mtime").toString(), Qt::ISODate);

                if ((ctime != file.created()) ||
                    (mtime != file.lastModified()))
                {
                    result.append(file.absoluteFilePath());
                }
            }
        }
    }

    return result;
}

bool RzUpdate::checkFile(const QString &filename)
{
    m_currentFilename = filename;
    int i = m_currentFilename.lastIndexOf('/');
    if (i != -1) {
        m_currentFilename = m_currentFilename.mid(i + 1);
    }
    m_skip = true;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QTextStream ts(&file);
    ts.setCodec(QTextCodec::codecForName("ISO-8859-1"));
    int lineCount = 0;
    resetOptions();
    QString id;
    while (!ts.atEnd()) {
        QString line = ts.readLine().trimmed();
        lineCount++;
        if (line.isEmpty() || (line[0] == '#')) {
            continue;
        }
        if (line.startsWith("Id=")) {
            id = m_currentFilename + ':' + line.mid(3);
        } else if (line.startsWith("File=")) {
            checkGotFile(line.mid(5), id);
        }
    }

    return true;
}

void RzUpdate::checkGotFile(const QString &_file, const QString &id)
{
    QString file;
    int i = _file.indexOf(',');
    if (i == -1) {
        file = _file.trimmed();
    } else {
        file = _file.mid(i + 1).trimmed();
    }

//   qDebug("File %s, id %s", file.toLatin1().constData(), id.toLatin1().constData());

    QSettings cfg(file, QSettings::IniFormat);
    QStringList ids = cfg.value(UPDATE_INFO_KEY).toStringList();
    if (ids.contains(id)) {
        return;
    }
    ids.append(id);
    cfg.setValue(UPDATE_INFO_KEY, ids);
}

// A path can either be relative or absolute, or starts from "~" (tilde).
QString RzUpdate::expandConfigFileName(const QString &file) const
{
    if (file.startsWith('/'))
        return file;

    if (file.startsWith("~/"))
        return QDir::homePath() + file.mid(1);

     return XdgDirs::configHome() + "/" + file;
}

/**
 * Syntax:
 * # Comment
 * Id=id
 * File=oldfile[,newfile]
 * AllGroups
 * Group=oldgroup[,newgroup]
 * RemoveGroup=oldgroup
 * Options=[copy,][overwrite,]
 * Key=oldkey[,newkey]
 * RemoveKey=ldkey
 * AllKeys
 * Keys= [Options](AllKeys|(Key|RemoveKey)*)
 * ScriptArguments=arguments
 * Script=scriptfile[,interpreter]
 *
 * Sequence:
 * (Id,(File(Group,Keys)*)*)*
 **/
bool RzUpdate::updateFile(const QString &filename)
{
    m_updFilePath = filename;
    m_currentFilename = filename;
    int i = m_currentFilename.lastIndexOf('/');
    if (i != -1) {
        m_currentFilename = m_currentFilename.mid(i + 1);
    }
    m_skip = true;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    //log() << "Checking update-file '" << filename << "' for new updates" << endl;
    QTextStream ts(&file);
    ts.setCodec(QTextCodec::codecForName("ISO-8859-1"));
    m_lineCount = 0;
    resetOptions();
    while (!ts.atEnd()) {
        m_line = ts.readLine().trimmed();
        m_lineCount++;
        log() << m_line;
        if (m_line.isEmpty() || (m_line[0] == '#'))
        {
            continue;
        }

        if (m_line.startsWith(QLatin1String("Id=")))
        {
            gotId(m_line.mid(3));
        }

        else if (m_skip)
        {
            continue;
        }

        else if (m_line.startsWith(QLatin1String("Options=")))
        {
            gotOptions(m_line.mid(8));
        }

        else if (m_line.startsWith(QLatin1String("File=")))
        {
            gotFile(m_line.mid(5));
        }

        else if (m_skipFile)
        {
            continue;
        }

        else if (m_line.startsWith(QLatin1String("Group=")))
        {
            gotGroup(m_line.mid(6));
        }

        else if (m_line.startsWith(QLatin1String("RemoveGroup=")))
        {
            gotRemoveGroup(m_line.mid(12));
            resetOptions();
        }

        else if (m_line.startsWith(QLatin1String("Script=")))
        {
            gotScript(m_line.mid(7));
            resetOptions();
        }

        else if (m_line.startsWith(QLatin1String("ScriptArguments=")))
        {
            gotScriptArguments(m_line.mid(16));
        }

        else if (m_line.startsWith(QLatin1String("Key=")))
        {
            gotKey(m_line.mid(4));
            resetOptions();
        }

        else if (m_line.startsWith(QLatin1String("RemoveKey=")))
        {
            gotRemoveKey(m_line.mid(10));
            resetOptions();
        }

        else if (m_line == "AllKeys")
        {
            gotAllKeys();
            resetOptions();
        }

        else if (m_line == "AllGroups")
        {
            gotAllGroups();
            resetOptions();
        }

        else
        {
            logFileError() << "Parse error" << endl;
        }
    }
    // Flush.
    gotId(QString());

    QFileInfo fi(filename);
    m_config->setValue(fi.fileName() + "/ctime", fi.created().toString(Qt::ISODate));
    m_config->setValue(fi.fileName() + "/mtime", fi.lastModified().toString(Qt::ISODate));
    m_config->sync();
    return true;
}



void RzUpdate::gotId(const QString &_id)
{
    if (!m_id.isEmpty() && !m_skip)
    {
        m_config->beginGroup(m_currentFilename);
        QStringList ids = m_config->value("done").toStringList();
        if (!ids.contains(m_id))
        {
            ids.append(m_id);
            m_config->setValue("done", ids);
            m_config->sync();
        }
        m_config->endGroup();
    }

    // Flush pending changes
    gotFile(QString());
    m_config->beginGroup(m_currentFilename);
    QStringList ids = m_config->value("done").toStringList();
    m_config->endGroup();

    if (!_id.isEmpty())
    {
        if (ids.contains(_id))
        {
            //qDebug("Id '%s' was already in done-list", _id.toLatin1().constData());
            if (!m_bUseConfigInfo)
            {
                m_skip = true;
                return;
            }
        }
        m_skip = false;
        m_skipFile = false;
        m_id = _id;
        if (m_bUseConfigInfo)
        {
            log() << m_currentFilename << ": Checking update '" << _id << "'" << endl;
        } else
        {
            log() << m_currentFilename << ": Found new update '" << _id << "'" << endl;
        }
    }
}

void RzUpdate::gotFile(const QString &_file)
{
    // Reset group
    gotGroup(QString());

    if (!m_oldFile.isEmpty())
    {
        QStringList ids = m_oldConfig->value(UPDATE_INFO_KEY).toStringList();
        QString cfg_id = m_currentFilename + ':' + m_id;
        if (!ids.contains(cfg_id) && !m_skip)
        {
            ids.append(cfg_id);
            m_oldConfig->setValue(UPDATE_INFO_KEY, ids);
        }
        m_oldConfig->sync();
        delete m_oldConfig;
        m_oldConfig = 0;


        QFileInfo file(XdgDirs::configHome() + "/" + m_oldFile);
        if (file.exists() && !file.size())
        {
            // Delete empty file.
            QFile::remove(file.absoluteFilePath());
        }

        m_oldFile.clear();
    }

    if (!m_newFile.isEmpty())
    {
        // Close new file.
        QStringList ids = m_newConfig->value(UPDATE_INFO_KEY).toStringList();
        QString cfg_id = m_currentFilename + ':' + m_id;
        if (!ids.contains(cfg_id) && !m_skip)
        {
            ids.append(cfg_id);
            m_newConfig->setValue(UPDATE_INFO_KEY, ids);
        }
        m_newConfig->sync();
        delete m_newConfig;
        m_newConfig = 0;

        m_newFile.clear();
    }
    m_newConfig = 0;

    int i = _file.indexOf(',');
    if (i == -1)
    {
        m_oldFile = _file.trimmed();
    }
    else
    {
        m_oldFile = _file.left(i).trimmed();
        m_newFile = _file.mid(i + 1).trimmed();
        if (m_oldFile == m_newFile)
        {
            m_newFile.clear();
        }
    }

    if (!m_oldFile.isEmpty())
    {
        m_oldConfig = new QSettings(expandConfigFileName(m_oldFile), QSettings::IniFormat);
        QString cfg_id = m_currentFilename + ':' + m_id;
        QStringList ids = m_oldConfig->value(UPDATE_INFO_KEY).toStringList();
        if (ids.contains(cfg_id))
        {
            m_skip = true;
            m_newFile.clear();
            log() << m_currentFilename << ": Skipping update '" << m_id << "'" << endl;
        }

        if (!m_newFile.isEmpty())
        {
            m_newConfig = new QSettings(expandConfigFileName(m_newFile), QSettings::IniFormat);
            ids = m_newConfig->value(UPDATE_INFO_KEY).toStringList();
            if (ids.contains(cfg_id))
            {
                m_skip = true;
                log() << m_currentFilename << ": Skipping update '" << m_id << "'" << endl;
            }
        }
        else
        {
            m_newConfig = m_oldConfig;
        }

        // A path can either be relative or absolute, or starts from "~" (tilde).
        QString path;
        if (m_oldFile.startsWith('/'))
        {
            path = m_oldFile;
        }
        else if (m_oldFile.startsWith("~/"))
        {
            path = QDir::homePath() + m_oldFile.mid(1);
        }
        else
        {
            path = XdgDirs::configHome() + "/" + m_oldFile;
        }
    }
    else
    {
        m_newFile.clear();
    }

    m_newFileName = m_newFile;
    if (m_newFileName.isEmpty())
    {
        m_newFileName = m_oldFile;
    }

    m_skipFile = false;
    if (!m_oldFile.isEmpty())
    {
        // if File= is specified, it doesn't exist, is empty or contains only kconf_update's [$Version] group, skip
        if (m_oldConfig != NULL)
        {
            if(!QFileInfo(m_oldConfig->fileName()).exists())
            {
                log() << m_currentFilename << ": File '" << m_oldFile << "' does not exist, skipping" << endl;
                m_skipFile = true;
            }
            else
            {
                QStringList keys = m_oldConfig->allKeys();
                if (keys.isEmpty() ||
                   (keys.count() == 1 && keys.first() == UPDATE_INFO_KEY))
                {
                    log() << m_currentFilename << ": File '" << m_oldFile << "' empty, skipping" << endl;
                    m_skipFile = true;
                }
            }
        }
    }
}

QString RzUpdate::unescapeString(const QString &src, bool *ok, QString *error)
{
    QString dst;
    int length = src.length();
    for (int pos = 0; pos < length; ++pos)
    {
        QChar ch = src.at(pos);
        if (ch != '\\')
        {
            dst += ch;
        }
        else
        {
            ++pos;
            if (pos == length)
            {
                *ok = false;
                *error = QString("Unfinished escape sequence in %1").arg(src);
                return QString();
            }
            ch = src.at(pos);
            if (ch == 's')
            {
                dst += ' ';
            }
            else if (ch == 't')
            {
                dst += '\t';
            }
            else if (ch == 'n')
            {
                dst += '\n';
            }
            else if (ch == 'r')
            {
                dst += '\r';
            }
            else if (ch == '\\')
            {
                dst += '\\';
            }
            else if (ch == 'x')
            {
                if (pos + 2 < length)
                {
                    char value = src.mid(pos + 1, 2).toInt(ok, 16);
                    if (*ok)
                    {
                        dst += QChar::fromAscii(value);
                        pos += 2;
                    }
                    else
                    {
                        *error = QString("Invalid hex escape sequence at column %1 in %2").arg(pos).arg(src);
                        return QString();
                    }
                }
                else
                {
                    *ok = false;
                    *error = QString("Unfinished hex escape sequence at column %1 in %2").arg(pos).arg(src);
                    return QString();
                }
            }
            else
            {
                *ok = false;
                *error = QString("Invalid escape sequence at column %1 in %2").arg(pos).arg(src);
                return QString();
            }
        }
    }

    *ok = true;
    return dst;
}


QString RzUpdate::parseGroupString(const QString &_str, bool *ok, QString *error)
{
    QString e;

    QString str = unescapeString(_str.trimmed(), ok, error);
    if (!ok)
    {
        return "";
    }

    *ok = true;

    if (str[0] != '[')
    {
        // Simplified notation, no '['
        return str;
    }

    if (!str.endsWith(']'))
    {
        *ok = false;
        *error = QString("Missing closing ']' in %1").arg(_str);
        return "";
    }

    // trim outer brackets
    str.chop(1);
    str.remove(0, 1);

    return str.replace("][", "/");
}

QString RzUpdate::parseGroupString(const QString &_str) const
{
    bool ok;
    QString error;
    QString result = parseGroupString(_str, &ok, &error);
    if (!ok)
    {
        logFileError() << error;
    }
    return result;
}

void RzUpdate::gotGroup(const QString &_group)
{
    QString group = _group.trimmed();
    if (group.isEmpty())
    {
        m_oldGroup.clear();
        m_newGroup.clear();
        return;
    }

    QStringList tokens = group.split(',');
    m_oldGroup = parseGroupString(tokens.at(0));
    if (tokens.count() == 1)
    {
        m_newGroup = m_oldGroup;
    }
    else
    {
        m_newGroup = parseGroupString(tokens.at(1));
    }
}


void RzUpdate::gotRemoveGroup(const QString &_group)
{
    m_oldGroup = parseGroupString(_group);

    if (!m_oldConfig)
    {
        logFileError() << "RemoveGroup without previous File specification" << endl;
        return;
    }

    // Delete group.
    m_oldConfig->beginGroup(m_oldGroup);
    m_oldConfig->remove("");
    m_oldConfig->endGroup();
    log() << m_currentFilename << ": RemoveGroup removes group " << m_oldFile << ":" << m_oldGroup << endl;
}


void RzUpdate::gotKey(const QString &_key)
{
    QString oldKey, newKey;
    int i = _key.indexOf(',');
    if (i == -1)
    {
        oldKey = _key.trimmed();
        newKey = oldKey;
    }
    else
    {
        oldKey = _key.left(i).trimmed();
        newKey = _key.mid(i + 1).trimmed();
    }

    if (oldKey.isEmpty() || newKey.isEmpty())
    {
        logFileError() << "Key specifies invalid key" << endl;
        return;
    }

    if (!m_oldConfig)
    {
        logFileError() << "Key without previous File specification" << endl;
        return;
    }
    copyOrMoveKey(m_oldGroup + "/" + oldKey, m_newGroup + "/" + newKey);
}

void RzUpdate::copyOrMoveKey(const QString &srcKey, const QString &dstKey)
{
    if (!m_bOverwrite && m_newConfig->contains(dstKey))
    {
        log() << m_currentFilename << ": Skipping " << m_newFileName << ":" << dstKey << ", already exists." << endl;
        return;
    }

    QVariant value = m_oldConfig->value(srcKey);
    log() << m_currentFilename << ": Updating " << m_newFileName << ":" << dstKey << " to '" << value.toString() << "'" << endl;
    m_newConfig->setValue(dstKey, value);

    if (m_bCopy)
    {
        return; // Done.
    }

    // Delete old entry
    if (m_oldConfig == m_newConfig
        && srcKey == dstKey)
    {
        return; // Don't delete!
    }

    m_oldConfig->remove(srcKey);
    log() << m_currentFilename << ": Removing " << m_oldFile << ":" << srcKey << ", moved." << endl;
}

void RzUpdate::copyOrMoveGroup(const QString &srcGroupPath, const QString &dstGroupPath)
{
    m_oldConfig->beginGroup(srcGroupPath);
    QStringList keys = m_oldConfig->allKeys();
    m_oldConfig->endGroup();

    foreach (QString key, keys)
    {
        copyOrMoveKey(srcGroupPath + "/" + key, dstGroupPath + "/" + key);
    }

}

void RzUpdate::gotRemoveKey(const QString &_key)
{
    QString key = _key.trimmed();

    if (key.isEmpty())
    {
        logFileError() << "RemoveKey specifies invalid key" << endl;
        return;
    }

    if (!m_oldConfig)
    {
        logFileError() << "Key without previous File specification" << endl;
        return;
    }


    if (!m_oldConfig->contains(m_oldGroup + "/" + key))
    {
        return;
    }

    log() << m_currentFilename << ": RemoveKey removes " << m_oldFile << ":" << m_oldGroup << ":" << key << endl;

    // Delete old entry
    m_oldConfig->beginGroup(m_oldGroup);
    m_oldConfig->remove("");
    m_oldConfig->endGroup();
}

void RzUpdate::gotAllKeys()
{
    if (!m_oldConfig)
    {
        logFileError() << "AllKeys without previous File specification" << endl;
        return;
    }
    copyOrMoveGroup(m_oldGroup, m_newGroup);
}

void RzUpdate::gotAllGroups()
{
    if (!m_oldConfig)
    {
        logFileError() << "AllGroups without previous File specification" << endl;
        return;
    }

    const QStringList allGroups = m_oldConfig->childGroups();
    foreach (const QString &group, allGroups)
    {
        m_oldGroup = group;
        m_newGroup = m_oldGroup;
        gotAllKeys();
    }
}

void RzUpdate::gotOptions(const QString &_options)
{
    const QStringList options = _options.split(',');
    for (QStringList::ConstIterator it = options.begin();
            it != options.end();
            ++it)
    {
        if ((*it).toLower().trimmed() == "copy")
        {
            m_bCopy = true;
        }

        if ((*it).toLower().trimmed() == "overwrite")
        {
            m_bOverwrite = true;
        }
    }
}


void RzUpdate::copyGroup(QSettings *srcCfg, const QString &srcGroup, QFile *outFile)
{
    srcCfg->beginGroup(srcGroup);
    QStringList keys = srcCfg->allKeys();
    srcCfg->endGroup();

    QTextStream out(outFile);
    QString group;
    foreach (QString key, keys)
    {
        QString srcKey  = QString("%1/%2").arg(srcGroup, key);
        if (key.contains('/'))
        {
            QString g = key.section('/', 0, 0);
            if (g != group)
            {
                group = g;
                out << "[" << group << "]\n";
            }

            key = key.section('/', 1);
        }

        key.replace('\\', '/');
        out << key << "=" << srcCfg->value(srcKey).toString() << "\n";
    }
}


void RzUpdate::gotScriptArguments(const QString &_arguments)
{
    m_arguments = _arguments;
}

bool customReadIni(QIODevice &device, QSettings::SettingsMap &map)
{
    QString group;
    QTextStream stream(&device);
    while (!stream.atEnd())
    {
        QString line = stream.readLine();

        if (line.isEmpty())
            continue;

        if (line.startsWith('#'))
            continue;

        if (line.startsWith('['))
        {
            bool ok;
            QString error;
            group = RzUpdate::parseGroupString(line, &ok, &error);
            continue;
        }

        if (!line.contains('='))
        {
            continue;
        }

        QString key = line.section('=', 0, 0).trimmed();
        key.replace('\\', '/');
        if (!group.isEmpty())
            key = group + "/" + key;

        QString value = line.section('=', 1).trimmed();

        // Remove quotes ........................
        if ((value.startsWith('"') && value.endsWith('"')) ||
            (value.startsWith('\'') && value.endsWith('\'')))
            value = value.mid(1, value.length()-2);

         map.insert(key, value);

    }
    return true;
}

 bool customWriteIni(QIODevice &device, const QSettings::SettingsMap &map)
 {
    return true;
 }

void RzUpdate::gotScript(const QString &_script)
{
    QString script, interpreter;
    int i = _script.indexOf(',');
    if (i == -1)
    {
        script = _script.trimmed();
    }
    else
    {
        script = _script.left(i).trimmed();
        interpreter = _script.mid(i + 1).trimmed();
    }


    if (script.isEmpty())
    {
        logFileError() << "Script fails to specify filename";
        m_skip = true;
        return;
    }

    QStringList dirs;
    dirs << QFileInfo(m_updFilePath).dir().absolutePath();
    dirs << "/usr/local/lib/razor-confupdate_bin";
    dirs << "/usr/local/lib" LIB_SUFFIX "/razor-confupdate_bin";
    dirs << "/usr/lib/razor-confupdate_bin";
    dirs << "/usr/lib" LIB_SUFFIX "/razor-confupdate_bin";

    QFileInfo file;
    foreach (const QString &dir, dirs)
    {
        file = QFileInfo(QString("%1/%2").arg(dir).arg(script));

        if (file.exists())
            break;
    }

    if (!file.exists())
    {
        logFileError() << "Script '" << script << "' not found" << endl;
        m_skip = true;
        return;
    }


    if (!m_arguments.isNull())
    {
        log() << m_currentFilename << ": Running script '" << script << "' with arguments '" << m_arguments << "'" << endl;
    }
    else
    {
        log() << m_currentFilename << ": Running script '" << script << "'" << endl;
    }

    QString cmd;
    if (interpreter.isEmpty())
    {
        cmd = file.absoluteFilePath();
    }
    else
    {
        cmd = interpreter + ' ' + file.absoluteFilePath();
    }

    if (!m_arguments.isNull())
    {
        cmd += ' ';
        cmd += m_arguments;
    }

    QTemporaryFile scriptIn(QDir::tempPath() + "/razor-confupdate_XXXXXX.in");
    scriptIn.open();

    QTemporaryFile scriptOut(QDir::tempPath() + "/razor-confupdate_XXXXXX.out");
    scriptOut.open();

    QTemporaryFile scriptErr(QDir::tempPath() + "/razor-confupdate_XXXXXX.err");
    scriptErr.open();

    int result;
    if (m_oldConfig)
    {
        if (m_debug)
        {
            scriptIn.setAutoRemove(false);
            log() << "Script input stored in " << scriptIn.fileName() << endl;
        }
        copyGroup(m_oldConfig, m_oldGroup, &scriptIn);

#ifndef _WIN32_WCE
        result = system(QFile::encodeName(QString("%1 < %2 > %3 2> %4").arg(cmd, scriptIn.fileName(), scriptOut.fileName(), scriptErr.fileName())));
#else
        QString path_ = QDir::convertSeparators ( QFileInfo ( cmd ).absoluteFilePath() );
        QString file_ = QFileInfo ( cmd ).fileName();
        SHELLEXECUTEINFO execInfo;
        memset ( &execInfo,0,sizeof ( execInfo ) );
        execInfo.cbSize = sizeof ( execInfo );
        execInfo.fMask =  SEE_MASK_FLAG_NO_UI;
        execInfo.lpVerb = L"open";
        execInfo.lpFile = (LPCWSTR) path_.utf16();
        execInfo.lpDirectory = (LPCWSTR) file_.utf16();
        execInfo.lpParameters = (LPCWSTR) QString(" < %1 > %2 2> %3").arg( scriptIn.fileName(), scriptOut.fileName(), scriptErr.fileName()).utf16();
        result = ShellExecuteEx ( &execInfo );
        if (result != 0)
        {
            result = 0;
        }
        else
        {
            result = -1;
        }
#endif
    }
    else
    {
        // No config file
#ifndef _WIN32_WCE
        result = system(QFile::encodeName(QString("%1 2> %2").arg(cmd, scriptErr.fileName())));
#else
        QString path_ = QDir::convertSeparators ( QFileInfo ( cmd ).absoluteFilePath() );
        QString file_ = QFileInfo ( cmd ).fileName();
        SHELLEXECUTEINFO execInfo;
        memset ( &execInfo,0,sizeof ( execInfo ) );
        execInfo.cbSize = sizeof ( execInfo );
        execInfo.fMask =  SEE_MASK_FLAG_NO_UI;
        execInfo.lpVerb = L"open";
        execInfo.lpFile = (LPCWSTR) path_.utf16();
        execInfo.lpDirectory = (LPCWSTR) file_.utf16();
        execInfo.lpParameters = (LPCWSTR) QString(" 2> %1").arg( scriptErr.fileName()).utf16();
        result = ShellExecuteEx ( &execInfo );
        if (result != 0)
        {
            result = 0;
        }
        else
        {
            result = -1;
        }
#endif
    }

    // Copy script stderr to log file
    {
        QFile output(scriptErr.fileName());
        if (output.open(QIODevice::ReadOnly))
        {
            QTextStream ts(&output);
            ts.setCodec(QTextCodec::codecForName("UTF-8"));
            while (!ts.atEnd()) {
                QString line = ts.readLine();
                log() << "[Script] " << line << endl;
            }
        }
    }

    if (result)
    {
        log() << m_currentFilename << ": !! An error occurred while running '" << cmd << "'" << endl;
        return;
    }

    if (!m_oldConfig)
    {
        return; // Nothing to merge
    }

    if (m_debug)
    {
        scriptOut.setAutoRemove(false);
        log() << "Script output stored in " << scriptOut.fileName() << endl;
    }

    // Deleting old entries
    {
        QString group = m_oldGroup;
        QFile output(scriptOut.fileName());
        if (output.open(QIODevice::ReadOnly))
        {
            QTextStream ts(&output);
            ts.setCodec(QTextCodec::codecForName("UTF-8"));
            while (!ts.atEnd())
            {
                QString line = ts.readLine();
                if (line.startsWith('['))
                {
                    group = parseGroupString(line);
                }

                else if (line.startsWith(QLatin1String("# DELETE ")))
                {
                    QString key = line.mid(9);
                    if (key[0] == '[')
                    {
                        int j = key.lastIndexOf(']') + 1;
                        if (j > 0)
                        {
                            group = parseGroupString(key.left(j));
                            key = key.mid(j);
                        }
                    }

                    m_oldConfig->remove(group + "/" + key);
                    log() << m_currentFilename << ": Script removes " << m_oldFile << ":" << group << ":" << key << endl;

                }

                else if (line.startsWith(QLatin1String("# DELETEGROUP")))
                {
                    QString str = line.mid(13).trimmed();
                    if (!str.isEmpty())
                    {
                        group = parseGroupString(str);
                    }
                    m_oldConfig->beginGroup(group);
                    m_oldConfig->remove("");
                    m_oldConfig->endGroup();
                    log() << m_currentFilename << ": Script removes group " << m_oldFile << ":" << group << endl;
                }
            }
        }
    }

    // Merging in new entries.
    const QSettings::Format CustomIniFormat = QSettings::registerFormat("", customReadIni, customWriteIni);
    QSettings cfgOut(scriptOut.fileName(), CustomIniFormat);
    QStringList keys = cfgOut.allKeys();

    foreach (const QString &key, keys)
    {
        QString dstKey = m_newGroup + "/" + key;
        if (!m_bOverwrite && m_newConfig->contains(dstKey))
        {
            log() << m_currentFilename << ": Skipping " << m_newFileName << ":" << dstKey << ", already exists." << endl;
            continue;
        }
        m_newConfig->setValue(dstKey, cfgOut.value(key));
    }
}

void RzUpdate::resetOptions()
{
    m_bCopy = false;
    m_bOverwrite = false;
    m_arguments.clear();
}

