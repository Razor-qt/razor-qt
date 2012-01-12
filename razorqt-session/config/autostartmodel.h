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

#ifndef AUTOSTARTMODEL_H
#define AUTOSTARTMODEL_H

#include <QtCore/QAbstractItemModel>

#include <qtxdg/xdgautostart.h>
#include <QtCore/QList>
#include <QtCore/QSet>

class AutoStartItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit AutoStartItemModel(QObject* parent = 0);
    ~AutoStartItemModel();
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QModelIndex parent(const QModelIndex &child) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    void updateData(const QModelIndex& index);
    bool removeRow(int row, const QModelIndex& parent = QModelIndex());
    void addEntry(const QModelIndex& index, XdgDesktopFile* entry);

public slots:
    bool writeChanges();

private:
    XdgDesktopFileList mAllItems;
    QPersistentModelIndex mGlobalIndex;
    QPersistentModelIndex mRazorIndex;
    QList<XdgDesktopFile*> mGlobalItems;
    QList<XdgDesktopFile*> mRazorItems;
    QSet<XdgDesktopFile*> mEditedItems;
    QSet<XdgDesktopFile*> mDeletedItems;
    bool showOnlyInRazor(const XdgDesktopFile& file) const;
};

#endif // AUTOSTARTMODEL_H
