/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
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

#include "iconthemeinfo.h"
#include <QtCore/QDebug>


#define PRIVIEW_ICON_SIZE 22


IconThemeInfo::IconThemeInfo(const QDir &dir):
    mValid(false),
    mHidden(false)
{
    mName = dir.dirName();
    if (dir.exists("index.theme"))
        load(dir.absoluteFilePath("index.theme"));
}


void IconThemeInfo::load(const QString &fileName)
{
    mFileName = fileName;
    mValid = false;
    QSettings file(mFileName, QSettings::IniFormat);
    if (file.status() != QSettings::NoError)
        return;

    if (file.value("Icon Theme/Directories").toStringList().join("").isEmpty())
        return;

    mHidden = file.value("Icon Theme/Hidden", false).toBool();
    mText = file.value("Icon Theme/Name").toString();
    mComment = file.value("Icon Theme/Comment").toString();
    mValid = true;
    loadDirsInfo(file, QFileInfo(mFileName).dir().canonicalPath());
}


void IconThemeInfo::loadDirsInfo(QSettings &file, const QString &path)
{
    foreach (QString i, file.value("Icon Theme/Directories", QStringList()).toStringList())
    {
        file.beginGroup(i);
        if (file.value("Size", 0).toInt() == PRIVIEW_ICON_SIZE &&
            file.value("Context").toString() == "Actions"
           )   
        {
            mActionsDir = path + QDir::separator() + i;
            file.endGroup();              
            return;
        }
        file.endGroup();
    }
}


QIcon IconThemeInfo::icon(const QString &iconName) const
{
    QDir dir(mActionsDir);

    if (dir.exists(iconName + ".png"))
    {
        QIcon icon(mActionsDir + QDir::separator() + iconName + ".png");
        return icon;
    }

    if (dir.exists(iconName + ".svg"))
    {
        QIcon icon(mActionsDir + QDir::separator() + iconName + ".svg");
        return icon;
    }

    if (dir.exists(iconName + ".xpm"))
    {
        QIcon icon(mActionsDir + QDir::separator() + iconName + ".xpm");
        return icon;
    }

    return QIcon();
}
