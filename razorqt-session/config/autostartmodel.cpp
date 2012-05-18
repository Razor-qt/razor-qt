/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright (C) 2011  Alec Moskvin <alecm@gmx.com>
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

#include <QtCore/QFileInfo>
#include <qtxdg/xdgdirs.h>
#include <qtxdg/xdgicon.h>

#include "autostartmodel.h"
#include <QDebug>

struct AutoStartItemModel::AutoStartItem
{
    // user's autostart file, overrides systemFile if it exists
    XdgDesktopFile* localFile;

    // desktop file outside user's autostart dir
    const XdgDesktopFile* systemFile;

    // true if localFile does not exist on disk yet
    bool tentative;

    AutoStartItem() : localFile(NULL), systemFile(NULL), tentative(false) {}
    const XdgDesktopFile* file() { return localFile ? localFile : systemFile; }
    bool overrides() { return systemFile && localFile; }
};

/*
 * Get a list of all autostart files in the user's autostart dir ("local files")
 * and then get all that are outside ("system files"). If a local file has the
 * same name as the system one, then it overrides it.
 *
 * All items are also split in two categories: Global (for all DE's) and Razor-only
 */
AutoStartItemModel::AutoStartItemModel(QObject* parent) :
    QAbstractItemModel(parent),
    mGlobalIndex(QAbstractItemModel::createIndex(0, 0)),
    mRazorIndex(QAbstractItemModel::createIndex(1, 0))
{
    mFileList = XdgAutoStart::desktopFileList(QStringList() << XdgDirs::autostartHome(), false);
    int homeCount = mFileList.size();
    mFileList << XdgAutoStart::desktopFileList(XdgDirs::autostartDirs(), false);

    XdgDesktopFileList::iterator i;
    for (i = mFileList.begin(); i != mFileList.end(); ++i)
    {
        XdgDesktopFile* file = &(*i);
        QString filePath = file->fileName();
        QString fileName = QFileInfo(filePath).fileName();
        AutoStartItem data;
        if (homeCount-- > 0)
            data.localFile = file;
        else
            data.systemFile = file;

        if (mItemMap.contains(fileName))
        {
            AutoStartItem* parent = &mItemMap[fileName];
            if (!parent->systemFile)
                parent->systemFile = file;
            continue;
        }
        mItemMap.insert(fileName, data);

        if (showOnlyInRazor(file))
            mRazorItems.append(&mItemMap[fileName]);
        else
            mGlobalItems.append(&mItemMap[fileName]);
    }
}

AutoStartItemModel::~AutoStartItemModel()
{
}

/*
 * All actual file operations are perfomed here when the window is closed.
 */
bool AutoStartItemModel::writeChanges()
{
    foreach (const XdgDesktopFile* file, mDeletedItems)
        QFile::remove(file->fileName());
    foreach (const XdgDesktopFile* file, mEditedItems)
        file->save(XdgAutoStart::localPath(*file));
    return true;
}

/*
 * Create a new autostart entry when the user clicks Add.
 * If an entry already exists return false.
 */
bool AutoStartItemModel::addEntry(const QModelIndex& index, XdgDesktopFile entry)
{
    QModelIndex parent;
    if (index.parent().isValid())
        parent = index.parent();
    else if (index.isValid())
        parent = index;
    else
        parent = mGlobalIndex;

    QString fileName = entry.fileName();
    if (mItemMap.contains(fileName))
        return false;

    if (parent == mRazorIndex)
        entry.setValue("OnlyShowIn", "Razor;");

    AutoStartItem data;
    createItem(&data, entry);
    mItemMap.insert(fileName, data);

    beginInsertRows(parent, 0, 0);
    if (parent == mGlobalIndex)
        mGlobalItems.append(&mItemMap[fileName]);
    else
        mRazorItems.append(&mItemMap[fileName]);
    endInsertRows();

    return true;
}

/*
 * Changes the data. If a "system" file is to be modified, a local copy is created.
 * If the local copy is identical to the system copy, the local one is deleted.
 */
bool AutoStartItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role == Qt::CheckStateRole || role == Qt::UserRole)
    {
        AutoStartItem* data = static_cast<AutoStartItem*>(index.internalPointer());
        XdgDesktopFile* file = localCopy(data);
        if (role == Qt::CheckStateRole)
        {
            if (value == Qt::Checked)
                file->removeEntry("Hidden");
            else
                file->setValue("Hidden", true);
        }
        mEditedItems.insert(file);

        if (data->overrides() && *data->localFile == *data->systemFile)
        {
            deleteItem(data);
        }

        emit dataChanged(index, index);
        return true;
    }
    return false;
}

/*
 * When user clicks "Delete", removes the local item. If there is a "system" item,
 * it replaces it.
 */
bool AutoStartItemModel::removeRow(int row, const QModelIndex& parent)
{
    if (!parent.isValid())
        return false;
    AutoStartItem* item;

    if (parent == mGlobalIndex)
        item = mGlobalItems[row];
    else
        item = mRazorItems[row];

    if (!item->localFile)
        return false;

    if (item->overrides())
    {
        deleteItem(item);

        QModelIndex index = parent.child(row, 0);
        emit dataChanged(index, index);
        return false;
    }

    beginRemoveRows(parent, row, row);
    if (parent == mGlobalIndex)
        mGlobalItems.removeAt(row);
    else
        mRazorItems.removeAt(row);
    endRemoveRows();

    deleteItem(item);
    return true;
}

QModelIndex AutoStartItemModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!parent.isValid())
    {
        if (row == 0)
            return mGlobalIndex;
        else if (row == 1)
            return mRazorIndex;
    }
    else if (parent == mGlobalIndex && row < mGlobalItems.size())
        return QAbstractItemModel::createIndex(row, column, mGlobalItems.at(row));
    else if (parent == mRazorIndex && row < mRazorItems.size())
        return QAbstractItemModel::createIndex(row, column, mRazorItems.at(row));
    return QModelIndex();
}

QVariant AutoStartItemModel::data(const QModelIndex& index, int role) const
{
    AutoStartItem* data = static_cast<AutoStartItem*>(index.internalPointer());
    if (role == Qt::DisplayRole)
    {
        if (index.parent().isValid())
            return data->file()->name();
        else if (index.row() == 0)
            return QString(tr("Global Autostart"));
        else if (index.row() == 1)
            return QString(tr("Razor Autostart"));
    }
    else if (!index.parent().isValid())
    {
        return QVariant();
    }
    else if (role == Qt::ToolTipRole)
    {
        QStringList tooltip;
        if (!data->tentative)
            tooltip << tr("Location: %1").arg(data->file()->fileName());
        if (data->overrides())
            tooltip << tr("Overrides: %1").arg(data->systemFile->fileName());
        return tooltip.join("\n");
    }
    else if (role == Qt::DecorationRole)
    {
        if (data->overrides())
            return XdgIcon::fromTheme("dialog-warning");
        else if (data->systemFile)
            return XdgIcon::fromTheme("computer");
    }
    else if (role == Qt::CheckStateRole)
    {
        bool hidden = data->file()->value("Hidden", false).toBool();
        return hidden ? Qt::Unchecked : Qt::Checked;
    }
    return QVariant();
}

Qt::ItemFlags AutoStartItemModel::flags(const QModelIndex& index) const
{
    if (index.parent().isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
    if (index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return 0;
}

/*
 * Controls which buttons are enabled/disabled based on the selecton.
 */
AutoStartItemModel::ActiveButtons AutoStartItemModel::activeButtons(const QModelIndex& selection)
{
    if (!selection.isValid() || !selection.parent().isValid())
        return AddButton;
    AutoStartItem* data = static_cast<AutoStartItem*>(selection.internalPointer());
    if (!data->localFile)
        return AddButton | EditButton;
    else
        return AddButton | EditButton | DeleteButton;
}

QModelIndex AutoStartItemModel::parent(const QModelIndex& child) const
{
    AutoStartItem* data = static_cast<AutoStartItem*>(child.internalPointer());
    if (data && data->file())
    {
        if (showOnlyInRazor(data->file()))
           return mRazorIndex;
        return mGlobalIndex;
    }
    return QModelIndex();
}

int AutoStartItemModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}

int AutoStartItemModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return 2;
    if (parent == mGlobalIndex)
        return mGlobalItems.size();
    if (parent == mRazorIndex)
        return mRazorItems.size();
    return 0;
}

bool AutoStartItemModel::showOnlyInRazor(const XdgDesktopFile* file)
{
    return file->value("OnlyShowIn") == "Razor;";
}

/*
 * Returns a the local desktop file, creating one if it does not exist
 */
XdgDesktopFile* AutoStartItemModel::localCopy(AutoStartItem* item)
{
    if (!item->localFile)
        createItem(item, *item->systemFile);
    return item->localFile;
}

/*
 * Convenience function to add a new item
 */
void AutoStartItemModel::createItem(AutoStartItem* item, const XdgDesktopFile& file)
{
    mFileList.append(XdgDesktopFile(file));
    item->localFile = &mFileList.last();
    item->tentative = true;
    mEditedItems.insert(item->localFile);
}

/*
 * Removes as much of an item as possible
 */
void AutoStartItemModel::deleteItem(AutoStartItem* item)
{
    if (!item->localFile)
        return;

    mEditedItems.remove(item->localFile);
    if (item->tentative)
    {
        if (!item->systemFile)
        {
            mItemMap.remove(item->file()->name());
            mFileList.removeOne(*item->file());
        }
        item->tentative = false;
    }
    else
    {
        mDeletedItems.insert(item->localFile);
    }
    item->localFile = NULL;
}

/*
 * Returns the local copy of the desktop file at the index.
 */
XdgDesktopFile* AutoStartItemModel::desktopFile(const QModelIndex& index)
{
    AutoStartItem* data = static_cast<AutoStartItem*>(index.internalPointer());
    if (!data)
        return NULL;
    if(data->localFile)
        return data->localFile;
    else
        return localCopy(data);
}
