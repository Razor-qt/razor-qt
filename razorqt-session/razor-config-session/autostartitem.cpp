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
    state(DEFAULT), system(false), local(false)
{
}

AutostartItem::AutostartItem(const XdgDesktopFile& systemFile) :
    systemFile(systemFile), state(DEFAULT), system(true), local(false)
{
}

AutostartItem::~AutostartItem()
{
}

void AutostartItem::setLocal(const XdgDesktopFile& file, bool real)
{
    if (local)
    {
        state = MODIFIED;
        if (system && file == systemFile)
            removeLocal();
        else
            localFile = file;
    }
    else
    {
        localFile = file;
        local = true;
        if (!real)
            state = TRANSIENT;
    }
}

bool AutostartItem::removeLocal()
{
    if (!local)
        return false;

    if (state == TRANSIENT)
        state = DEFAULT;
    else
        state = DELETED;

    local = false;
    return !system;
}

const XdgDesktopFile& AutostartItem::file() const
{
    return local ? localFile : systemFile;
}

const XdgDesktopFile& AutostartItem::systemfile() const
{
    return systemFile;
}

void AutostartItem::setEnabled(bool enable)
{
    XdgDesktopFile f = file();
    if (enable)
        f.removeEntry("Hidden");
    else
        f.setValue("Hidden", true);

    setLocal(f);
}

bool AutostartItem::isEnabled() const
{
    return !file().value("Hidden", false).toBool();
}

bool AutostartItem::isEmpty() const
{
    return !system && !local && (state == TRANSIENT || state != DELETED);
}

bool AutostartItem::commit()
{
    if (state == DELETED)
    {
        state = DEFAULT;
        return QFile::remove(localFile.fileName());
    }
    else if (state == MODIFIED || state == TRANSIENT)
    {
        state = DEFAULT;
        return localFile.save(XdgAutoStart::localPath(localFile));
    }
    return true;
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
            items[name].setLocal(file, true);
        }
        else
        {
            AutostartItem item;
            item.setLocal(file, true);
            items.insert(name, item);
        }
    }
    return items;
}
