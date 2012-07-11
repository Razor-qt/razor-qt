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

#ifndef RZ_UPDATE_H
#define RZ_UPDATE_H

#include <QtCore/QString>
class QStringList;
class QTextStream;
class QSettings;
class QFile;


class RzUpdate
{
public:
    RzUpdate();
    ~RzUpdate();
    void run(const QStringList &_files);

    bool isDebug() const { return m_debug; }
    void setDebug(bool _value) { m_debug = _value; }

    QString argCheckFile() const { return m_argCheckFile; }
    void setArgCheckFile(const QString &_checkFile) { m_argCheckFile = _checkFile; }

    QStringList findUpdateFiles(bool dirtyOnly);

    QTextStream &log() const;
    QTextStream &logFileError() const;

    bool checkFile(const QString &filename);
    void checkGotFile(const QString &_file, const QString &id);

    bool updateFile(const QString &filename);

    void gotId(const QString &_id);
    void gotFile(const QString &_file);
    void gotGroup(const QString &_group);
    void gotRemoveGroup(const QString &_group);
    void gotKey(const QString &_key);
    void gotRemoveKey(const QString &_key);
    void gotAllKeys();
    void gotAllGroups();
    void gotOptions(const QString &_options);
    void gotScript(const QString &_script);
    void gotScriptArguments(const QString &_arguments);
    void resetOptions();

    void copyGroup(QSettings *cfg1, const QString &group1, QFile *out);
    void copyOrMoveKey(const QString &srcKey, const QString &dstKey);
    void copyOrMoveGroup(const QString &srcGroupPath, const QString &dstGroupPath);

    static QString parseGroupString(const QString &_str, bool *ok, QString *error);
    static QString unescapeString(const QString &src, bool *ok, QString *error);
    QString parseGroupString(const QString &_str) const;

    QString expandConfigFileName(const QString &file) const;
protected:
    QSettings *m_config;
    QString m_currentFilename;
    QString m_updFilePath;
    bool m_skip;
    bool m_skipFile;
    bool m_debug;
    QString m_id;

    QString m_oldFile;
    QString m_newFile;
    QString m_newFileName;

    //QSettings *m_oldConfig1; // Config to read keys from.
    QSettings *m_oldConfig;
    QSettings *m_newConfig;

    QString m_oldGroup;
    QString m_newGroup;

    bool m_bCopy;
    bool m_bOverwrite;
    bool m_bUseConfigInfo;
    QString m_arguments;
    QFile *m_file;
    QString m_line;
    int m_lineCount;
    QString m_argCheckFile;
};

#endif //RZ_UPDATE_H
