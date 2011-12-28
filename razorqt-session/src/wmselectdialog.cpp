/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL3+
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


#include "wmselectdialog.h"
#include "ui_wmselectdialog.h"
#include "windowmanager.h"
#include <QtGui/QListView>
#include <QtCore/QVariant>
#include <stdlib.h>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>


WmSelectDialog::WmSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WmSelectDialog)
{
    ui->setupUi(this);
    setModal(true);
    connect(ui->wmList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(accept()));
    
    QMapIterator<QString,QString> it(availableWindowManagers());
    while (it.hasNext())
    {
        it.next();
        addWindowManager(it.key(), it.value());
    }
}

WmSelectDialog::~WmSelectDialog()
{
    delete ui;
}


QString WmSelectDialog::windowManager() const
{
    QListWidgetItem *item = ui->wmList->currentItem();
    if (item)
        return item->data(Qt::UserRole).toString();

    return QString();
}

void WmSelectDialog::addWindowManager(const QString &program, const QString &description)
{
     QListWidgetItem *item = new QListWidgetItem(description, ui->wmList);
     item->setData(Qt::UserRole, QVariant(program));
     bool enable = findProgram(program);
     if (!enable)
        item->setFlags(Qt::NoItemFlags);

     if (enable && ui->wmList->currentRow() < 0)
     {
        ui->wmList->setCurrentItem(item);
     }
}
