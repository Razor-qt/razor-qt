/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2013 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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

#ifndef GLOBAL_ACTION_MANAGER__DEFAULT_MODEL__INCLUDED
#define GLOBAL_ACTION_MANAGER__DEFAULT_MODEL__INCLUDED


#include <QAbstractTableModel>
#include <QMap>

#include "../daemon/meta_types.hpp"


class Actions;


class DefaultModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DefaultModel(Actions *, QObject *parent = 0);
    ~DefaultModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

signals:
    
public slots:
    void daemonDisappeared();
    void daemonAppeared();

    void actionAdded(qulonglong id);
    void actionEnabled(qulonglong id, bool enabled);
    void actionModified(qulonglong id);
    void actionsSwapped(qulonglong id1, qulonglong id2);
    void actionRemoved(qulonglong id);

private:
    Actions *m_actions;
    QMap<qulonglong, GeneralActionInfo> m_content;

};

#endif // GLOBAL_ACTION_MANAGER__DEFAULT_MODEL__INCLUDED
