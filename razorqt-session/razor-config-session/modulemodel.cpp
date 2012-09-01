/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright (C) 2012  Alec Moskvin <alecm@gmx.com>
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

#include "modulemodel.h"

ModuleModel::ModuleModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

void ModuleModel::reset()
{
    mKeyList.clear();
    mItemMap = AutostartItem::createItemMap();
    QMap<QString,AutostartItem>::iterator iter;
    for (iter = mItemMap.begin(); iter != mItemMap.end(); ++iter)
    {
        if (iter.value().file().value("X-Razor-Module", false).toBool())
            mKeyList.append(iter.key());
    }
}

QVariant ModuleModel::data(const QModelIndex& index, int role) const
{
    QString name = mKeyList.at(index.row());
    if (index.column() == 0)
    {
        switch (role)
        {
            case Qt::DisplayRole:
                return mItemMap.value(name).file().name();
            case Qt::CheckStateRole:
                return mItemMap.value(name).isEnabled() ? Qt::Checked : Qt::Unchecked;
            case Qt::ToolTipRole:
                return mItemMap.value(name).file().value("Comment");
        }
    }
    return QVariant();
}

bool ModuleModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role == Qt::CheckStateRole)
    {
        QString key = mKeyList.at(index.row());
        mItemMap[key].setEnabled(value == Qt::Checked);
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

Qt::ItemFlags ModuleModel::flags(const QModelIndex& index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
}

int ModuleModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return mKeyList.size();
}

void ModuleModel::writeChanges()
{
    foreach (const QString& key, mKeyList)
        mItemMap[key].commit();
}
