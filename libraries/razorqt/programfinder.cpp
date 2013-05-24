/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright (C) 2013  Alec Moskvin <alecm@gmx.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "programfinder.h"
#include <wordexp.h>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

bool ProgramFinder::programExists(const QString& command)
{
    QString program = programName(command);
    if (program[0] == QChar('/'))
    {
        QFileInfo fi(program);
        return fi.isExecutable() && fi.isFile();
    }

    QString path = qgetenv("PATH");
    foreach (const QString& dirName, path.split(":", QString::SkipEmptyParts))
    {
        QFileInfo fi(QDir(dirName), program);
        if (fi.isExecutable() && fi.isFile())
            return true;
    }
    return false;
}

QStringList ProgramFinder::findPrograms(const QStringList& programs)
{
    QStringList availPrograms;
    foreach (const QString& program, programs)
        if (programExists(program))
            availPrograms.append(program);
    return availPrograms;
}

QString ProgramFinder::programName(const QString& command)
{
    wordexp_t we;
    if (wordexp(command.toLocal8Bit().constData(), &we, WRDE_NOCMD) == 0)
        if (we.we_wordc > 0)
            return QString(we.we_wordv[0]);
    return QString();
}
