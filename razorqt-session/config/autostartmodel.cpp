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

#include <QtCore/QFile>

#include "autostartmodel.h"

AutoStartItemModel::AutoStartItemModel(QObject* parent) :
    QAbstractItemModel(parent),
    mGlobalIndex(QAbstractItemModel::createIndex(0, 0)),
    mRazorIndex(QAbstractItemModel::createIndex(1, 0))
{
    mXdgAutoStart = new XdgAutoStart(false);
    foreach (XdgDesktopFile* file, mXdgAutoStart->list())
    {
        if (showOnlyInRazor(file))
            mRazorItems.append(file);
        else
            mGlobalItems.append(file);
    }
}

AutoStartItemModel::~AutoStartItemModel()
{
    delete mXdgAutoStart;
}

bool AutoStartItemModel::writeChanges()
{
    foreach (XdgDesktopFile* file, mDeletedItems)
        QFile::remove(file->fileName());
    foreach (XdgDesktopFile* file, mEditedItems)
        file->save(XdgAutoStart::localPath(file));
    return true;
}

void AutoStartItemModel::addEntry(const QModelIndex& index, XdgDesktopFile* entry)
{
    QModelIndex parent;
    if (index.parent().isValid())
        parent = index.parent();
    else if (index.isValid())
        parent = index;
    else
        parent = mGlobalIndex;

    beginInsertRows(parent, 0, 0);
    if (parent == mGlobalIndex)
    {
        mGlobalItems.append(entry);
    }
    else
    {
        entry->setValue("OnlyShowIn", "RAZOR;");
        mRazorItems.append(entry);
    }
    endInsertRows();

    mEditedItems.insert(entry);
}

bool AutoStartItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    XdgDesktopFile* file = static_cast<XdgDesktopFile*>(index.internalPointer());
    if (role == Qt::CheckStateRole)
    {
        file->setValue("Hidden", value == Qt::Checked ? "false" : "true");
        mEditedItems.insert(file);
        return true;
    }
    else if (role == Qt::UserRole)
    {
        mEditedItems.insert(file);
        return true;
    }
    return false;
}

bool AutoStartItemModel::removeRow(int row, const QModelIndex& parent)
{
    if (!parent.isValid())
        return false;
    XdgDesktopFile* deleted;

    beginRemoveRows(parent, row, row);
    if (parent == mGlobalIndex)
        deleted = mGlobalItems.takeAt(row);
    else
        deleted = mRazorItems.takeAt(row);
    endRemoveRows();

    mEditedItems.remove(deleted);
    mDeletedItems.insert(deleted);
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
    XdgDesktopFile* file = static_cast<XdgDesktopFile*>(index.internalPointer());
    if (role == Qt::DisplayRole)
    {
        if (index.parent().isValid())
            return file->name();
        else if (index.row() == 0)
            return QString("Global Autostart");
        else if (index.row() == 1)
            return QString("Razor Autostart");
    }
    else if (role == Qt::CheckStateRole && index.parent().isValid())
        return file->value("Hidden").toBool() ? Qt::Unchecked : Qt::Checked;
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


QModelIndex AutoStartItemModel::parent(const QModelIndex& child) const
{
    XdgDesktopFile* file = static_cast<XdgDesktopFile*>(child.internalPointer());
    if (file)
    {
        if (showOnlyInRazor(file))
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

bool AutoStartItemModel::showOnlyInRazor(XdgDesktopFile* file) const
{
    return file->value("OnlyShowIn") == "RAZOR;";
}
