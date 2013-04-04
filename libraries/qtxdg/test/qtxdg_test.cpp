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

#include "qtxdg_test.h"

#include "qtxdg/xdgdesktopfile.h"
#include "qtxdg/xdgdirs.h"

#include <QDir>
#include <QFileInfo>
#include <QProcess>

#include <QDebug>

void QtXdgTest::testDefaultApp()
{
    QStringList mimedirs = XdgDirs::dataDirs();
    mimedirs.prepend(XdgDirs::dataHome(false));
    foreach (QString mimedir, mimedirs)
    {
        QDir dir(mimedir + "/mime");
        qDebug() << dir.path();
        QStringList filters = (QStringList() << "*.xml");
        foreach(QFileInfo mediaDir, dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
        {
            qDebug() << "    " << mediaDir.fileName();
            foreach (QString mimeXmlFileName, QDir(mediaDir.absoluteFilePath()).entryList(filters, QDir::Files))
            {
                QString mimetype = mediaDir.fileName() + "/" + mimeXmlFileName.left(mimeXmlFileName.length() - 4);
                
                QString xdg_utils_default = xdgUtilDefaultApp(mimetype);
                QString desktop_file_default = xdgDesktopFileDefaultApp(mimetype);
                
                if (xdg_utils_default != desktop_file_default) 
                {
                    qDebug() << mimetype;
                    qDebug() << "xdg-mime query default:" << xdg_utils_default;
                    qDebug() << "xdgdesktopfile default:" << desktop_file_default;
                }
            }
        }
         
    }
}

void QtXdgTest::compare(QString mimetype)
{
    QString xdgUtilDefault = xdgUtilDefaultApp(mimetype);
    QString xdgDesktopDefault = xdgDesktopFileDefaultApp(mimetype);
    if (xdgUtilDefault != xdgDesktopDefault) 
    {
        qDebug() << mimetype;
        qDebug() << "xdg-mime default:" << xdgUtilDefault;
        qDebug() << "xdgDesktopfile default:" << xdgDesktopDefault;
    }
}


void QtXdgTest::testTextHtml()
{
    compare("text/html");
}

void QtXdgTest::testMeldComparison()
{
    compare("application/x-meld-comparison");
}


QString QtXdgTest::xdgDesktopFileDefaultApp(QString mimetype)
{
    XdgDesktopFile *defaultApp = XdgDesktopFileCache::getDefaultApp(mimetype); 
    QString defaultAppS;
    if (defaultApp)
    {
        defaultAppS = QFileInfo(defaultApp->fileName()).fileName();
    } 
    return defaultAppS;
}

QString QtXdgTest::xdgUtilDefaultApp(QString mimetype)
{
    QProcess xdg_mime;
    QString program = "xdg-mime";
    QStringList args = (QStringList() << "query" << "default" << mimetype);
    qDebug() << "running" << program << args.join(" ");
    xdg_mime.start(program, args);
    xdg_mime.waitForFinished(1000);
    return QString(xdg_mime.readAll()).trimmed();
}