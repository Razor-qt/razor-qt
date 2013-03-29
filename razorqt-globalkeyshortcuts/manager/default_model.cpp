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


#include "default_model.hpp"
#include "actions.hpp"


DefaultModel::DefaultModel(Actions *actions, QObject *parent)
    : QAbstractTableModel(parent)
    , m_actions(actions)
{
    connect(actions, SIGNAL(daemonDisappeared()), SLOT(daemonDisappeared()));
    connect(actions, SIGNAL(daemonAppeared()), SLOT(daemonAppeared()));
    connect(actions, SIGNAL(actionAdded(qulonglong)), SLOT(actionAdded(qulonglong)));
    connect(actions, SIGNAL(actionModified(qulonglong)), SLOT(actionModified(qulonglong)));
    connect(actions, SIGNAL(actionEnabled(qulonglong,bool)), SLOT(actionEnabled(qulonglong,bool)));
    connect(actions, SIGNAL(actionsSwapped(qulonglong,qulonglong)), SLOT(actionsSwapped(qulonglong,qulonglong)));
    connect(actions, SIGNAL(actionRemoved(qulonglong)), SLOT(actionRemoved(qulonglong)));
}

DefaultModel::~DefaultModel()
{
}

int DefaultModel::rowCount(const QModelIndex &/*parent*/) const
{
    return m_content.size();
}

int DefaultModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 4;
}

QVariant DefaultModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
        if ((index.row() >= 0) && (index.row() < rowCount()) && (index.column() >= 0) && (index.column() < columnCount()))
            switch (index.column())
            {
            case 0:
                return m_content[m_content.keys()[index.row()]].shortcut;

            case 1:
                return m_content[m_content.keys()[index.row()]].description;

            case 2:
                return m_content[m_content.keys()[index.row()]].type;

            case 3:
                return m_content[m_content.keys()[index.row()]].info;
            }
        break;

    case Qt::CheckStateRole:
        if ((index.row() >= 0) && (index.row() < rowCount()) && (index.column() == 0))
            return m_content[m_content.keys()[index.row()]].enabled;
        break;

    default:;
    }

    return QVariant();
}

QVariant DefaultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
        switch (orientation)
        {
        case Qt::Horizontal:
            switch (section)
            {
            case 0:
                return tr("Shortcut");

            case 1:
                return tr("Description");

            case 2:
                return tr("Type");

            case 3:
                return tr("Info");
            }
            break;

        default:;
        }
        break;

    default:;
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags DefaultModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags result = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (index.column() == 0)
        result |= Qt::ItemIsUserCheckable;
    return result;
}

void DefaultModel::daemonDisappeared()
{
    beginRemoveRows(QModelIndex(), 0, rowCount());

    m_content.clear();

    endRemoveRows();
}

void DefaultModel::daemonAppeared()
{
    QList<qulonglong> allIds = m_actions->allActionIds();

    beginInsertRows(QModelIndex(), 0, allIds.size() - 1);

    foreach (qulonglong id, allIds)
        m_content[id] = m_actions->actionById(id).second;

    endInsertRows();
}

void DefaultModel::actionAdded(qulonglong id)
{
    if (!m_content.contains(id))
    {
        QPair<bool, GeneralActionInfo> result = m_actions->actionById(id);
        if (result.first)
        {
            QMap<qulonglong, GeneralActionInfo>::ConstIterator I = m_content.lowerBound(id);
            int row = (I == m_content.constEnd()) ? m_content.size() : m_content.keys().indexOf(I.key());

            beginInsertRows(QModelIndex(), row, row);

            m_content[id] = result.second;

            endInsertRows();
        }
    }
}

void DefaultModel::actionEnabled(qulonglong id, bool enabled)
{
    if (m_content.contains(id))
    {
        int row = m_content.keys().indexOf(id);

        m_content[id].enabled = enabled;

        emit dataChanged(createIndex(row, 0), createIndex(row, 3));
    }
}

void DefaultModel::actionModified(qulonglong id)
{
    if (m_content.contains(id))
    {
        QPair<bool, GeneralActionInfo> result = m_actions->actionById(id);
        if (result.first)
        {
            int row = m_content.keys().indexOf(id);

            m_content[id] = result.second;

            emit dataChanged(createIndex(row, 0), createIndex(row, 3));
        }
    }
}

void DefaultModel::actionsSwapped(qulonglong id1, qulonglong id2)
{
    if (m_content.contains(id1) && m_content.contains(id2))
    {
        QList<qulonglong> keys = m_content.keys();
        int row1 = keys.indexOf(id1);
        int row2 = keys.indexOf(id2);

        std::swap(m_content[id1], m_content[id2]);

        emit dataChanged(createIndex(row1, 0), createIndex(row1, 3));
        emit dataChanged(createIndex(row2, 0), createIndex(row2, 3));
    }
}

void DefaultModel::actionRemoved(qulonglong id)
{
    if (m_content.contains(id))
    {
        int row = m_content.keys().indexOf(id);

        beginRemoveRows(QModelIndex(), row, row);

        m_content.remove(id);

        endRemoveRows();
    }
}
