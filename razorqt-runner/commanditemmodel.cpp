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
#include <limits.h>


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

void CommandItemModel::clearHistory()
{
    mSourceModel->clearHistory();
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
    if (left == mSourceModel->customCommandIndex())
        return true;

    if (right == mSourceModel->customCommandIndex())
        return false;

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
    unsigned int rank = 0;

    int cnt = rowCount();

    for (int i=0; i<cnt; ++i)
    {
        QModelIndex ind = index(i,0);
        QModelIndex srcIndex = mapToSource(ind);
        if (srcIndex == mSourceModel->customCommandIndex())
            continue;

        const CommandProviderItem *item = mSourceModel->command(srcIndex);
        if (!item)
            continue;

        unsigned int r = item->rank(pattern);
        if (r > rank)
        {
            res = ind;
            rank = r;
        }

        if (rank >= MAX_RANK)
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
    mCustomCommandProvider = new CustomCommandProvider;
    mProviders.append(mCustomCommandProvider);
    rebuild();
    mCustomCommandIndex = index(0, 0);

    mHistoryProvider = new HistoryProvider();
    mProviders.append(mHistoryProvider);
    mCustomCommandProvider->setHistoryProvider(mHistoryProvider);

    mProviders.append(new AppLinkProvider());
#ifdef MATH_ENABLED
    mProviders.append(new MathProvider());
#endif
#ifdef VBOX_ENABLED
    mProviders.append(new VirtualBoxProvider());
#endif

    mProviders.append(new PowerProvider());

    foreach(CommandProvider* provider, mProviders)
    {
        connect(provider, SIGNAL(changed()), this, SIGNAL(layoutChanged()));
        connect(provider, SIGNAL(aboutToBeChanged()), this, SIGNAL(layoutAboutToBeChanged()));
    }

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
    int ret=0;
    foreach(CommandProvider* provider, mProviders)
        ret += provider->count();

    return ret;
}


/************************************************

 ************************************************/
QVariant CommandSourceItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= rowCount())
        return QVariant();

    const CommandProviderItem *item = command(index);
    if (!item)
        return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
        return QString("<b>%1</b><br>\n%2\n").arg(item->title(), item->comment());

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
    QListIterator<CommandProvider*> i(mProviders);
    while (i.hasNext())
    {
        CommandProvider *p = i.next();
        if (p->isOutDated())
            p->rebuild();
    }
    emit layoutChanged();
}

void CommandSourceItemModel::clearHistory()
{
    mHistoryProvider->clearHistory();
    reset();
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
