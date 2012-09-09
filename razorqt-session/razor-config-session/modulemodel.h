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

#ifndef MODULEMODEL_H
#define MODULEMODEL_H

#include <QtGui/QStringListModel>
#include <QtDBus/QDBusInterface>
#include "autostartitem.h"

class ModuleModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ModuleModel(QObject *parent = 0);
    ~ModuleModel();
    void reset();
    void writeChanges();
    void toggleModule(const QModelIndex &index, bool status);

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role);
    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex&) const { return 2; }
    Qt::ItemFlags flags(const QModelIndex& index) const;

private slots:
    void updateModuleState(QString moduleName, bool state);

private:
    QMap<QString,AutostartItem> mItemMap;
    QMap<QString,bool> mStateMap;
    QStringList mKeyList;
    QDBusInterface* mInterface;
};

#endif // MODULEMODEL_H
