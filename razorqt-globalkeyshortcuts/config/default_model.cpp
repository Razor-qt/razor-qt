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


#include "default_model.h"
#include "actions.h"


DefaultModel::DefaultModel(Actions *actions, const QColor &grayedOutColour, const QFont &highlightedFont, const QFont &italicFont, const QFont &highlightedItalicFont, QObject *parent)
    : QAbstractTableModel(parent)
    , mActions(actions)
    , mGrayedOutColour(grayedOutColour)
    , mHighlightedFont(highlightedFont)
    , mItalicFont(italicFont)
    , mHighlightedItalicFont(highlightedItalicFont)
{
    connect(actions, SIGNAL(daemonDisappeared()), SLOT(daemonDisappeared()));
    connect(actions, SIGNAL(daemonAppeared()), SLOT(daemonAppeared()));
    connect(actions, SIGNAL(actionAdded(qulonglong)), SLOT(actionAdded(qulonglong)));
    connect(actions, SIGNAL(actionModified(qulonglong)), SLOT(actionModified(qulonglong)));
    connect(actions, SIGNAL(actionEnabled(qulonglong, bool)), SLOT(actionEnabled(qulonglong, bool)));
    connect(actions, SIGNAL(actionsSwapped(qulonglong, qulonglong)), SLOT(actionsSwapped(qulonglong, qulonglong)));
    connect(actions, SIGNAL(actionRemoved(qulonglong)), SLOT(actionRemoved(qulonglong)));

    mVerboseType["command"] = tr("Command");
    mVerboseType["method"] = tr("DBus call");
    mVerboseType["client"] = tr("Client");
}

DefaultModel::~DefaultModel()
{
}

int DefaultModel::rowCount(const QModelIndex &/*parent*/) const
{
    return mContent.size();
}

int DefaultModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 5;
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
                return mContent.keys()[index.row()];

            case 1:
                return mContent[mContent.keys()[index.row()]].shortcut;

            case 2:
                return mContent[mContent.keys()[index.row()]].description;

            case 3:
                return mVerboseType[mContent[mContent.keys()[index.row()]].type];

            case 4:
                return mContent[mContent.keys()[index.row()]].info;
            }
        break;

    case Qt::EditRole:
        if ((index.row() >= 0) && (index.row() < rowCount()) && (index.column() >= 0) && (index.column() < columnCount()))
            switch (index.column())
            {
            case 1:
                return mContent[mContent.keys()[index.row()]].shortcut;
            }
        break;

    case Qt::FontRole:
    {
        if ((index.row() >= 0) && (index.row() < rowCount()))
        {
            qulonglong id = mContent.keys()[index.row()];
            bool multiple = (index.column() == 1) && (mShortcuts[mContent[id].shortcut].size() > 1);
            bool inactive = (mContent[id].type == "client") && (mActions->getClientActionSender(id).isEmpty());
            if (multiple || inactive)
                return multiple ? (inactive ? mHighlightedItalicFont : mHighlightedFont) : mItalicFont;
        }
    }
        break;

    case Qt::ForegroundRole:
        if (!mContent[mContent.keys()[index.row()]].enabled)
        {
            return mGrayedOutColour;
        }
        break;

    case Qt::CheckStateRole:
        if ((index.row() >= 0) && (index.row() < rowCount()) && (index.column() == 0))
        {
            return mContent[mContent.keys()[index.row()]].enabled ? Qt::Checked : Qt::Unchecked;
        }
        break;

    default:
        ;
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
                return tr("Id");

            case 1:
                return tr("Shortcut");

            case 2:
                return tr("Description");

            case 3:
                return tr("Type");

            case 4:
                return tr("Info");
            }
            break;

        default:
            ;
        }
        break;

    default:
        ;
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags DefaultModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags result = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (index.column() == 0)
    {
        result |= Qt::ItemIsUserCheckable;
    }
    if (index.column() == 1)
    {
        result |= Qt::ItemIsEditable;
    }
    return result;
}

bool DefaultModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role)
    {
    case Qt::EditRole:
        if ((index.row() >= 0) && (index.row() < rowCount()) && index.column() == 1)
        {
            mActions->changeShortcut(mContent.keys()[index.row()], value.toString());
            return true;
        }
        break;
    }
    return false;
}

qulonglong DefaultModel::id(const QModelIndex &index) const
{
    if ((index.row() >= 0) && (index.row() < rowCount()))
    {
        return mContent.keys()[index.row()];
    }
    return 0ull;
}

void DefaultModel::daemonDisappeared()
{
    beginResetModel();

    mContent.clear();
    mShortcuts.clear();

    endResetModel();
}

void DefaultModel::daemonAppeared()
{
    QList<qulonglong> allIds = mActions->allActionIds();

    beginInsertRows(QModelIndex(), 0, allIds.size() - 1);

    foreach(qulonglong id, allIds)
    {
        mContent[id] = mActions->actionById(id).second;
        mShortcuts[mContent[id].shortcut].insert(id);
    }

    endInsertRows();
}

void DefaultModel::actionAdded(qulonglong id)
{
    if (!mContent.contains(id))
    {
        QPair<bool, GeneralActionInfo> result = mActions->actionById(id);
        if (result.first)
        {
            QList<qulonglong> keys = mContent.keys();
            int row = qBinaryFind(keys, mContent.lowerBound(id).key()) - keys.constBegin();

            beginInsertRows(QModelIndex(), row, row);

            mContent[id] = result.second;
            mShortcuts[mContent[id].shortcut].insert(id);

            endInsertRows();

            keys = mContent.keys();
            foreach(qulonglong siblingId, mShortcuts[mContent[id].shortcut])
            {
                if (id != siblingId)
                {
                    int siblingRow = qBinaryFind(keys, siblingId) - keys.constBegin();
                    emit dataChanged(index(siblingRow, 1), index(siblingRow, 1));
                }
            }
        }
    }
}

void DefaultModel::actionEnabled(qulonglong id, bool enabled)
{
    if (mContent.contains(id))
    {
        QList<qulonglong> keys = mContent.keys();
        int row = qBinaryFind(keys, id) - keys.constBegin();

        mContent[id].enabled = enabled;

        emit dataChanged(index(row, 0), index(row, 3));
    }
}

void DefaultModel::actionModified(qulonglong id)
{
    if (mContent.contains(id))
    {
        QPair<bool, GeneralActionInfo> result = mActions->actionById(id);
        if (result.first)
        {
            QList<qulonglong> keys = mContent.keys();
            int row = qBinaryFind(keys, id) - keys.constBegin();

            if (mContent[id].shortcut != result.second.shortcut)
            {
                mShortcuts[result.second.shortcut].insert(id);
                mShortcuts[mContent[id].shortcut].remove(id);
                foreach(qulonglong siblingId, mShortcuts[mContent[id].shortcut])
                {
                    int siblingRow = qBinaryFind(keys, siblingId) - keys.constBegin();
                    emit dataChanged(index(siblingRow, 1), index(siblingRow, 1));
                }
                foreach(qulonglong siblingId, mShortcuts[result.second.shortcut])
                {
                    int siblingRow = qBinaryFind(keys, siblingId) - keys.constBegin();
                    emit dataChanged(index(siblingRow, 1), index(siblingRow, 1));
                }
            }

            mContent[id] = result.second;

            emit dataChanged(index(row, 0), index(row, 3));
        }
    }
}

void DefaultModel::actionsSwapped(qulonglong id1, qulonglong id2)
{
    if (mContent.contains(id1) && mContent.contains(id2))
    {
        QList<qulonglong> keys = mContent.keys();
        int row1 = qBinaryFind(keys, id1) - keys.constBegin();
        int row2 = qBinaryFind(keys, id2) - keys.constBegin();

        std::swap(mContent[id1], mContent[id2]);

        emit dataChanged(index(row1, 0), index(row1, 3));
        emit dataChanged(index(row2, 0), index(row2, 3));
    }
}

void DefaultModel::actionRemoved(qulonglong id)
{
    if (mContent.contains(id))
    {
        QList<qulonglong> keys = mContent.keys();
        int row = qBinaryFind(keys, id) - keys.constBegin();

        beginRemoveRows(QModelIndex(), row, row);

        mShortcuts[mContent[id].shortcut].remove(id);
        QString shortcut = mContent[id].shortcut;

        mContent.remove(id);

        endRemoveRows();

        foreach(qulonglong siblingId, mShortcuts[shortcut])
        {
            int siblingRow = qBinaryFind(keys, siblingId) - keys.constBegin();
            emit dataChanged(index(siblingRow, 1), index(siblingRow, 1));
        }
    }
}
