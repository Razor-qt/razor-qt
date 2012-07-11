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

#include "test_rzupdate.h"

#include <qtxdg/xdgdirs.h>

// Qt
#include <QtTest/QtTest>
#include <QtCore/QFile>
#include <QtCore/QSharedPointer>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDebug>


static void writeFile(const QString &path, const QString &content)
{
    QFile file(path);
    bool ok = file.open(QIODevice::WriteOnly);
    Q_UNUSED(ok);
    Q_ASSERT(ok);
    file.write(content.toUtf8());
    file.close();
}

static QString readFile(const QString &path)
{
    Q_ASSERT(QFileInfo(path).exists());

    QFile file(path);
    file.open(QIODevice::ReadOnly);

    QMap<QString, QStringList> content;
    QString group;
    QStringList groups;
    QTextStream stream(&file);
    while (!stream.atEnd())
    {
        QString line = stream.readLine();
        if (line.isEmpty())
            continue;

        if (line.startsWith('['))
        {
            group = line;
            groups << line;
            continue;
        }

        content[group] << line;
    }

    QStringList result;
    groups.sort();
    foreach (const QString &group, groups)
    {
        QStringList keys=content[group];
        keys.sort();
        result << group;
        result << keys.join("\n");
    }

    return result.join("\n") + "\n";
}

static QTemporaryFile* writeUpdFile(const QString &content)
{
    QTemporaryFile* file = new QTemporaryFile("XXXXXX.upd");
    bool ok = file->open();
    Q_UNUSED(ok);
    Q_ASSERT(ok);
    file->write(content.toUtf8());
    file->flush();
    return file;
}

static void runKConfUpdate(const QString &updPath)
{
    QStringList dirs;
    dirs << ".";
    dirs << "..";
    dirs << QProcessEnvironment::systemEnvironment().value("PATH").split(':',QString::SkipEmptyParts);

    QString program;
    foreach (const QString &dir, dirs)
    {
        QFileInfo file(dir + "/razor-confupdate");
        if (file.exists() && file.isExecutable())
        {
            program = file.absoluteFilePath();
            break;
        }
    }

    if (program.isEmpty())
    {
        qWarning() << "Can't find razor-confupdate program.";
        return;
    }


    QStringList args;
    args << "--debug";
    args << updPath;

    int res = QProcess::execute(program, args);
    if (res != 0)
    {
        qWarning() << QString("Something wrong razor-confupdate return '%1'").arg(res);
    }
}

void TestRzUpdate::test_data()
{
    QTest::addColumn<QString>("updContent");
    QTest::addColumn<QString>("oldConfName");
    QTest::addColumn<QString>("oldConfContent");
    QTest::addColumn<QString>("newConfName");
    QTest::addColumn<QString>("expectedNewConfContent");
    QTest::addColumn<QString>("expectedOldConfContent");

    QTest::newRow("moveKeysSameFile")
        <<
           "File=testrc\n"
           "Group=group\n"
           "Key=old,new\n"
           "Options=overwrite\n"
        <<
           "testrc"
        <<
           "[group]\n"
           "old=value\n"
        <<
           "testrc"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[group]\n"
           "new=value\n"
        <<
        ""
        ;

    QTest::newRow("moveKeysOtherFile")
        <<
           "File=oldrc,newrc\n"
           "Group=group1,group2\n"
           "Key=old,new\n"
           "Options=overwrite\n"
        <<
           "oldrc"
        <<
           "[group1]\n"
           "old=value\n"
           "[stay]\n"
           "foo=bar\n"
        <<
           "newrc"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[group2]\n"
           "new=value\n"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[stay]\n"
           "foo=bar\n"
       ;

    QTest::newRow("allKeys")
        <<
           "File=testrc\n"
           "Group=group1,group2\n"
           "AllKeys\n"
           "Group=group3,group4\n"
           "AllKeys\n"
        <<
           "testrc"
        <<
           "[group1]\n"
           "key1=value1\n"
           "key2=value2\n"
           "[stay]\n"
           "foo=bar\n"
           "[group3]\n"
           "key1=group3_Value1\n"
           "key2=group3_Value2\n"
           "[group4]\n"
           "key1=group4_Value1\n"
        <<
           "testrc"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[group2]\n"
           "key1=value1\n"
           "key2=value2\n"
           "[group3]\n"
           "key1=group3_Value1\n"
           "[group4]\n"
           "key1=group4_Value1\n"
           "key2=group3_Value2\n"
           "[stay]\n"
           "foo=bar\n"
        <<
           ""
        ;
    QTest::newRow("allKeysCopy")
        <<
           "File=testrc\n"
           "Options=Copy\n"
           "Group=group1,group2\n"
           "AllKeys\n"
        <<
           "testrc"
        <<
           "[group1]\n"
           "key1=value1\n"
           "key2=value2\n"
           "[stay]\n"
           "foo=bar\n"
        <<
           "testrc"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[group1]\n"
           "key1=value1\n"
           "key2=value2\n"
           "[group2]\n"
           "key1=value1\n"
           "key2=value2\n"
           "[stay]\n"
           "foo=bar\n"
        <<
           ""
        ;
    QTest::newRow("allKeysOverride")
        <<
           "File=testrc\n"
           "Options=overwrite\n"
           "Group=group1,group2\n"
           "AllKeys\n"
        <<
           "testrc"
        <<
           "[group1]\n"
           "key1=newValue1\n"
           "key2=newValue2\n"
           "[stay]\n"
           "foo=bar\n"
           "[group2]\n"
           "key1=oldValue\n"
        <<
           "testrc"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[group2]\n"
           "key1=newValue1\n"
           "key2=newValue2\n"
           "[stay]\n"
           "foo=bar\n"
        <<
           ""
        ;

    QTest::newRow("allKeysSubGroup")
        <<
           "File=testrc\n"
           "Group=[group][sub1],[group][sub2]\n"
           "AllKeys\n"
        <<
           "testrc"
        <<
           "[group]\n"
           "sub1\\key1=value1\n"
           "sub1\\key2=value2\n"
           "sub1\\subsub\\key3=value3\n"
           "sub1\\subsub\\key4=value4\n"
           "[stay]\n"
           "foo=bar\n"
        <<
           "testrc"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[group]\n"
           "sub2\\key1=value1\n"
           "sub2\\key2=value2\n"
           "sub2\\subsub\\key3=value3\n"
           "sub2\\subsub\\key4=value4\n"
           "[stay]\n"
           "foo=bar\n"
        <<
        ""
        ;

    QTest::newRow("removeGroup")
        <<
           "File=testrc\n"
           "RemoveGroup=remove\n"
        <<
           "testrc"
        <<
           "[keep]\n"
           "key=value\n"
           ""
           "[remove]\n"
           "key=value\n"
        <<
           "testrc"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[keep]\n"
           "key=value\n"
        <<
           ""
        ;
}

void TestRzUpdate::test()
{
    QFETCH(QString, updContent);
    QFETCH(QString, oldConfName);
    QFETCH(QString, oldConfContent);
    QFETCH(QString, newConfName);
    QFETCH(QString, expectedNewConfContent);
    QFETCH(QString, expectedOldConfContent);

    // Prepend the Id= field to the upd content
    updContent = QString("Id=%1\n").arg(QTest::currentDataTag()) + updContent;
    QString oldConfPath = XdgDirs::configHome() + "/" + oldConfName;
    QString newConfPath = XdgDirs::configHome() + "/" + newConfName;

    QFile::remove(oldConfPath);
    QFile::remove(newConfPath);

    writeFile(oldConfPath, oldConfContent);
    QSharedPointer<QTemporaryFile> updFile(writeUpdFile(updContent));
    runKConfUpdate(updFile->fileName());

    QString updateInfo = QString("%1:%2")
        .arg(updFile->fileName().section('/', -1))
        .arg(QTest::currentDataTag());

    QString newConfContentAfter = '\n' + readFile(newConfPath);
    expectedNewConfContent = '\n' + expectedNewConfContent.arg(updateInfo);
    QCOMPARE(newConfContentAfter, expectedNewConfContent);

    if (oldConfName != newConfName)
    {
        QString oldConfContentAfter = readFile(oldConfPath);
        expectedOldConfContent = expectedOldConfContent.arg(updateInfo);
        QCOMPARE(oldConfContentAfter, expectedOldConfContent);
    }
}


void TestRzUpdate::testScript_data()
{
    QTest::addColumn<QString>("updContent");
    QTest::addColumn<QString>("updScript");
    QTest::addColumn<QString>("oldConfContent");
    QTest::addColumn<QString>("expectedNewConfContent");

    QTest::newRow("delete-key")
        <<
           "File=testrc\n"
           "Group=group\n"
           "Script=test.sh,sh\n"
        <<
           "echo '# DELETE deprecated'\n"
        <<
           "[group]\n"
           "deprecated=foo\n"
           "valid=bar\n"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[group]\n"
           "valid=bar\n"
           ;

    QTest::newRow("delete-key2")
        <<
           "File=testrc\n"
           "Script=test.sh,sh\n"
        <<
           "echo '# DELETE [group]deprecated'\n"
           "echo '# DELETE [group][sub]deprecated2'\n"
        <<
           "[group]\n"
           "deprecated=foo\n"
           "valid=bar\n"
           "[group]\n"
           "sub\\deprecated2=foo\n"
           "sub\\valid2=bar\n"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[group]\n"
           "sub\\valid2=bar\n"
           "valid=bar\n"
           ;

    QTest::newRow("delete-group")
        <<
           "File=testrc\n"
           "Script=test.sh,sh\n"
        <<
           "echo '# DELETEGROUP [group1]'\n"
           "echo '# DELETEGROUP [group2][sub]'\n"
        <<
           "[group1]\n"
           "key=value\n"
           "[group2]\n"
           "valid=bar\n"
           "[group2]\n"
           "sub\\key=value\n"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[group2]\n"
           "valid=bar\n"
           ;

    QTest::newRow("delete-group2")
        <<
           "File=testrc\n"
           "Group=group\n"
           "Script=test.sh,sh\n"
        <<
           "echo '# DELETEGROUP'\n"
        <<
           "[group]\n"
           "key=value\n"
           "[group2]\n"
           "valid=bar\n"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[group2]\n"
           "valid=bar\n"
           ;

    QTest::newRow("new-key")
        <<
           "File=testrc\n"
           "Script=test.sh,sh\n"
        <<
           "echo '[group]'\n"
           "echo 'new=value'\n"
        <<
           "[group]\n"
           "valid=bar\n"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[group]\n"
           "new=value\n"
           "valid=bar\n"
           ;

    QTest::newRow("modify-key-no-overwrite")
        <<
           "File=testrc\n"
           "Script=test.sh,sh\n"
        <<
           "echo '[group]'\n"
           "echo 'existing=new'\n"
        <<
           "[group]\n"
           "existing=old\n"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[group]\n"
           "existing=old\n"
           ;

    QTest::newRow("modify-key-overwrite")
        <<
           "File=testrc\n"
           "Options=overwrite\n"
           "Script=test.sh,sh\n"
        <<
           "echo '[group]'\n"
           "echo 'existing=new'\n"
        <<
           "[group]\n"
           "existing=old\n"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[group]\n"
           "existing=new\n"
           ;

    QTest::newRow("new-key-in-subgroup")
        <<
           "File=testrc\n"
           "Script=test.sh,sh\n"
        <<
           "echo '[group][sub]'\n"
           "echo 'new=value2'\n"
        <<
           "[group]\n"
           "sub\\existing=foo\n"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[group]\n"
           "sub\\existing=foo\n"
           "sub\\new=value2\n"
           ;

    QTest::newRow("new-key-in-subgroup2")
        <<
           "File=testrc\n"
           "Script=test.sh,sh\n"
        <<
           "echo '[group/sub]'\n"
           "echo 'new=value3'\n"
        <<
           "[group]\n"
           "sub\\existing=foo\n"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[group]\n"
           "sub\\existing=foo\n"
           "sub\\new=value3\n"
           ;

    QTest::newRow("filter")
        <<
           "File=testrc\n"
           "Script=test.sh,sh\n"
        <<
           "echo '# DELETE [group]changed'\n"
           "sed s/value/VALUE/\n"
        <<
           "[group]\n"
           "changed=value\n"
           "unchanged=value\n"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[group]\n"
           "changed=VALUE\n"
           "unchanged=value\n"
           ;

    QTest::newRow("filter2")
        <<
           "File=testrc\n"
           "Group=group\n"
           "Script=test.sh,sh\n"
        <<
           "echo '# DELETEGROUP [group]'\n"
           "echo '[newGroup]'\n"
           "sed s/value/VALUE/\n"
        <<
           "[group]\n"
           "changed=value\n"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[group]\n"
           "newGroup\\changed=VALUE\n"
           ;

    QTest::newRow("filter3")
        <<
           "File=testrc\n"
           "Group=group,newGroup\n"
           "Script=test.sh,sh\n"
        <<
           "echo '# DELETEGROUP [group]'\n"
           "sed s/value/VALUE/\n"
        <<
           "[group]\n"
           "changed=value\n"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[newGroup]\n"
           "changed=VALUE\n"
           ;

    QTest::newRow("filter-subgroup")
        <<
           "File=testrc\n"
           "Script=test.sh,sh\n"
        <<
           "echo '# DELETE [group][sub]changed'\n"
           "sed s/value/VALUE/\n"
        <<
           "[group]\n"
           "unchanged=value\n"
           "sub\\changed=value\n"
           "sub\\unchanged=value\n"
        <<
           "[__Version__]\n"
           "update_info=%1\n"
           "[group]\n"
           "sub\\changed=VALUE\n"
           "sub\\unchanged=value\n"
           "unchanged=value\n"
           ;
}

void TestRzUpdate::testScript()
{
    QFETCH(QString, updContent);
    QFETCH(QString, updScript);
    QFETCH(QString, oldConfContent);
    QFETCH(QString, expectedNewConfContent);

    // Prepend the Id= field to the upd content
    updContent = QString("Id=%1\n").arg(QTest::currentDataTag()) + updContent;

    QSharedPointer<QTemporaryFile> updFile(writeUpdFile(updContent));

    QString scriptPath = QFileInfo(updFile.data()->fileName()).absolutePath() + "/test.sh";
    writeFile(scriptPath, updScript);

    QString confPath = XdgDirs::configHome() + "/testrc";
    writeFile(confPath, oldConfContent);

    runKConfUpdate(updFile->fileName());
    QString updateInfo = QString("%1:%2")
        .arg(updFile->fileName().section('/', -1))
        .arg(QTest::currentDataTag());
    QString newConfContent = '\n' + readFile(confPath);
    expectedNewConfContent = '\n' + expectedNewConfContent.arg(updateInfo);
    QCOMPARE(newConfContent, expectedNewConfContent);
}

QTEST_MAIN(TestRzUpdate)
//#include "test_kconf_update.moc"
