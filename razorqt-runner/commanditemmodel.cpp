/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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

#include "commanditemmodel.h"


#include <qtxdg/xdgicon.h>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QDebug>


/************************************************

 ************************************************/
CommandItemModel::CommandItemModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    mSourceModel(new CommandSourceItemModel(this))
{
    setSourceModel(mSourceModel);
}


/************************************************

 ************************************************/
CommandItemModel::~CommandItemModel()
{
}



/************************************************

 ************************************************/
bool CommandItemModel::isOutDated() const
{
    return mSourceModel->isOutDated();
}


/************************************************

 ************************************************/
const CommandProviderItem *CommandItemModel::command(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    QModelIndex ind = mapToSource(index);

    return mSourceModel->command(ind);
}


/************************************************

 ************************************************/
void CommandItemModel::addHistoryCommand(const QString &command)
{
    mSourceModel->addHistoryCommand(command);
}


/************************************************

 ************************************************/
bool CommandItemModel::filterAcceptsRow(int sourceRow, const QModelIndex &/*sourceParent*/) const
{
    QRegExp re(filterRegExp());

    if (re.isEmpty() && !mOnlyHistory)
        return false;

    const CommandProviderItem *item = mSourceModel->command(sourceRow);

    if (!item)
        return false;

    return item->compare(re);
}


/************************************************

 ************************************************/
bool CommandItemModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (mOnlyHistory)
        return left.row() < right.row();
    else
        return QSortFilterProxyModel::lessThan(left, right);
}


/************************************************

 ************************************************/
QModelIndex CommandItemModel::appropriateItem(const QString &pattern) const
{
    QModelIndex res;
    int delta = 0xFFFF;

    int cnt = rowCount();
    for (int i=0; i<cnt; ++i)
    {
        QModelIndex ind = index(i,0);
        QModelIndex srcIndex = mapToSource(ind);
        const CommandProviderItem *item = mSourceModel->command(srcIndex);
        if (!item)
            continue;

        int d = item->tile().indexOf(pattern, 0, Qt::CaseInsensitive);
        if (d<delta)
        {
            res = ind;
            delta = d;
        }

        if (delta==0)
            break;
    }

    if (!res.isValid())
        res = index(0, 0);

    return res;
}


/************************************************

 ************************************************/
void CommandItemModel::rebuild()
{
    mSourceModel->rebuild();
}




/************************************************

 ************************************************/
CommandSourceItemModel::CommandSourceItemModel(QObject *parent) :
    QAbstractListModel(parent)
{
    mHistoryProvider = new HistoryProvider();
    mProviders.append(mHistoryProvider);
    mProviders.append(new AppLinkProvider());
#ifdef MATH_ENABLED
    mProviders.append(new MathProvider());
#endif
#ifdef VBOX_ENABLED
    mProviders.append(new VirtualBoxProvider());
#endif
    rebuild();
}


/************************************************

 ************************************************/
CommandSourceItemModel::~CommandSourceItemModel()
{
    qDeleteAll(mProviders);
    mHistoryProvider = 0;
}


/************************************************

 ************************************************/
int CommandSourceItemModel::rowCount(const QModelIndex& /*parent*/) const
{
    return mRowCount;
}


/************************************************

 ************************************************/
QVariant CommandSourceItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= mRowCount)
        return QVariant();

    const CommandProviderItem *item = command(index);
    if (!item)
        return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
        return QString("<b>%1</b><br>\n%2\n").arg(item->tile(), item->comment());

    case Qt::DecorationRole:
        return item->icon();

    case Qt::ToolTipRole:
        return item->toolTip();

    }

    return QVariant();
}


/************************************************

 ************************************************/
bool CommandSourceItemModel::isOutDated() const
{
    QListIterator<CommandProvider*> i(mProviders);
    while (i.hasNext())
    {
        if (i.next()->isOutDated())
            return true;
    }

    return false;
}


/************************************************

 ************************************************/
void CommandSourceItemModel::rebuild()
{
    int cnt = 0;
    QListIterator<CommandProvider*> i(mProviders);
    while (i.hasNext())
    {
        CommandProvider *p = i.next();
        if (p->isOutDated())
            p->rebuild();

        cnt += p->length();
    }
    mRowCount = cnt;
    emit layoutChanged();
}


/************************************************

 ************************************************/
const CommandProviderItem *CommandSourceItemModel::command(int row) const
{
    int n = row;
    QListIterator<CommandProvider*> i(mProviders);
    while (i.hasNext())
    {
        CommandProvider *p = i.next();
        if (n < p->count())
            return p->at(n);
        else
            n -=p->count();
    }

    return 0;
}


/************************************************

 ************************************************/
const CommandProviderItem *CommandSourceItemModel::command(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return command(index.row());
}


/************************************************

 ************************************************/
void CommandSourceItemModel::addHistoryCommand(const QString &command)
{
    mHistoryProvider->AddCommand(command);
}
