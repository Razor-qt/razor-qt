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

#include "shortcut_delegate.h"
#include "shortcut_selector.h"
#include "actions.h"

#include <QTimer>


ShortcutDelegate::ShortcutDelegate(Actions *actions, QObject *parent)
    : QItemDelegate(parent)
    , mActions(actions)
{
}

QWidget *ShortcutDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
    ShortcutSelector *button = new ShortcutSelector(mActions, parent);
    button->autoApplyShortcut();
    QTimer::singleShot(0, button, SLOT(click()));
    return button;
}

void ShortcutDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    ShortcutSelector *button = static_cast<ShortcutSelector*>(editor);
    if (button->isGrabbing())
        button->cancelNow();
    model->setData(index, button->text(), Qt::EditRole);
}

void ShortcutDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

void ShortcutDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    static_cast<QAbstractButton*>(editor)->setText(index.model()->data(index, Qt::EditRole).toString());
}

QSize ShortcutDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize sizeHint = QItemDelegate::sizeHint(option , index);
    sizeHint.setHeight(sizeHint.height() + 15);
    return sizeHint;
}
