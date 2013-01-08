/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright (C) 2012  Alec Moskvin <alecm@gmx.com>
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

#include "autostartitem.h"
#include <qtxdg/xdgautostart.h>
#include <qtxdg/xdgdirs.h>
#include <QtCore/QFileInfo>

AutostartItem::AutostartItem() :
    RazorAutostartEntry()
{
}

AutostartItem::AutostartItem(const XdgDesktopFile& systemFile) :
    RazorAutostartEntry()
{
    mSystemFile = systemFile;
    mSystem = true;
}

void AutostartItem::setLocalFromFile(const XdgDesktopFile& file)
{
    mLocalFile = file;
    mLocalState = StateExists;
}

QMap<QString,AutostartItem> AutostartItem::createItemMap()
{
    QMap<QString,AutostartItem> items;

    XdgDesktopFileList systemList = XdgAutoStart::desktopFileList(XdgDirs::autostartDirs(), false);
    foreach (const XdgDesktopFile& file, systemList)
    {
        QString name = QFileInfo(file.fileName()).fileName();
        items.insert(name, AutostartItem(file));
    }

    XdgDesktopFileList localList = XdgAutoStart::desktopFileList(QStringList(XdgDirs::autostartHome()), false);
    foreach (const XdgDesktopFile& file, localList)
    {
        QString name = QFileInfo(file.fileName()).fileName();
        if (items.contains(name))
        {
            items[name].setLocalFromFile(file);
        }
        else
        {
            AutostartItem item;
            item.setLocalFromFile(file);
            items.insert(name, item);
        }
    }
    return items;
}
