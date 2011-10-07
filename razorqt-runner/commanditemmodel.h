/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
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

#ifndef COMMANDITEMMODEL_H
#define COMMANDITEMMODEL_H

#include <providers.h>
#include <QtGui/QSortFilterProxyModel>
#include <QtCore/QAbstractListModel>
#include <QtCore/QVariant>


class CommandSourceItemModel: public QAbstractListModel
{
    Q_OBJECT
public:
    explicit CommandSourceItemModel(QObject *parent = 0);
    virtual ~CommandSourceItemModel();

    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

    bool isOutDated() const;
    const CommandProviderItem *command(const QModelIndex &index) const;
    const CommandProviderItem *command(int row) const;

    void addHistoryCommand(const QString &command);

public slots:
    void rebuild();


private:
    QList<CommandProvider*> mProviders;
    HistoryProvider *mHistoryProvider;
    int mRowCount;
};


class CommandItemModel: public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit CommandItemModel(QObject *parent = 0);
    virtual ~CommandItemModel();

    bool isOutDated() const;
    const CommandProviderItem *command(const QModelIndex &index) const;

    void addHistoryCommand(const QString &command);

    QModelIndex  appropriateItem(const QString &pattern) const;

    bool isShowOnlyHistory() const { return mOnlyHistory; }
    void showOnlyHistory(bool onlyHistory) { mOnlyHistory = onlyHistory; }

public slots:
    void rebuild();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    CommandSourceItemModel *mSourceModel;
    bool mOnlyHistory;
};


#endif // COMMANDITEMMODEL_H

