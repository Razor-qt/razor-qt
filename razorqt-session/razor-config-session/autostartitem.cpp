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
    mLocalState(StateNone), mSystem(false)
{
}

AutostartItem::AutostartItem(const XdgDesktopFile& systemFile) :
    mSystemFile(systemFile), mLocalState(StateNone), mSystem(true)
{
}

AutostartItem::~AutostartItem()
{
}

void AutostartItem::setLocal(const XdgDesktopFile& file)
{
    bool local = isLocal();
    if (mSystem && local && file == mSystemFile)
    {
        removeLocal();
    }
    else
    {
        if (local)
            mLocalState = StateModified;
        else
            mLocalState = StateTransient;
        mLocalFile = file;
    }
}

void AutostartItem::setLocalFromFile(const XdgDesktopFile& file)
{
    mLocalFile = file;
    mLocalState = StateExists;
}

bool AutostartItem::removeLocal()
{
    if (!isLocal())
        return false;

    if (mLocalState == StateTransient)
        mLocalState = StateNone;
    else
        mLocalState = StateDeleted;

    return !mSystem;
}

const XdgDesktopFile& AutostartItem::file() const
{
    return isLocal() ? mLocalFile : mSystemFile;
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

bool AutostartItem::commit()
{
    if (mLocalState == StateDeleted)
    {
        mLocalState = StateNone;
        return QFile::remove(mLocalFile.fileName());
    }
    else if (mLocalState == StateModified || mLocalState == StateTransient)
    {
        mLocalState = StateExists;
        return mLocalFile.save(XdgAutoStart::localPath(mLocalFile));
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
