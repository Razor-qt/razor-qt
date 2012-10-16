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

#include <QtCore/QCoreApplication>
#include <QtCore/QTextStream>
#include <QtCore/QString>
//#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>

#include <qtxdg/xdgdirs.h>
#include <qtxdg/xdgautostart.h>

XdgDesktopFileList desktopFiles;

XdgDesktopFile *findDesktopFile(const QString &program)
{
    XdgDesktopFileList::iterator i;
    for (i = desktopFiles.begin(); i != desktopFiles.end(); ++i)
    {
        if ((*i).value("Exec") == program)
        {
            return &(*i);
        }
    }

    return 0;
}

void processModule(const QString &program, bool enable)
{
    XdgDesktopFile *file = findDesktopFile(program);

    if (file)
    {
        // Change value in the file .............
        if (file->value("Hidden").toBool() == enable)
        {
            file->setValue("Hidden", !enable);
            file->save(XdgAutoStart::localPath(*file));
        }
    }
    else
    {
        // Create the new file ..................
        QString fileName = program;
        fileName.replace(QRegExp("[ \\\\/|]", Qt::CaseSensitive, QRegExp::RegExp), "_");
        XdgDesktopFile file;
        file.setValue("X-Razor-Module", true);
        file.setValue("OnlyShowIn", "Razor;");
        file.setValue("Type", "Application");
        file.setValue("Exec", program);
        file.setValue("Name", program);
        file.setValue("Hidden", !enable);
        file.save(QString("%1/%2.desktop").arg(XdgDirs::autostartHome(true), fileName));
    }
}

int main(int, char**)
{
    desktopFiles = XdgAutoStart::desktopFileList(false);

    QTextStream in(stdin);
    QString line;

    while (!in.atEnd())
    {
        line = in.readLine();
        if (!line.contains('='))
        {
            continue;
        }

        QString key = line.section('=', 0, 0).trimmed();
        QString value = line.section('=', 1).trimmed();

        processModule(key, value.toUpper().contains("TRUE") || value.toUpper().contains("Yes"));
    }

    return 0;
}
