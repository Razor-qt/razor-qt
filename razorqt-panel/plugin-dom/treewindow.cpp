/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2013 Razor team
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


#include "treewindow.h"
#include "ui_treewindow.h"
#include "domtreeitem.h"

#include <QDebug>
#include <QTableWidget>

#define PROP_OBJECT_NAME    0
#define PROP_CLASS_NAME     1
#define PROP_TEXT           2
#define PROP_CLASS_HIERARCY 3


/************************************************

 ************************************************/
TreeWindow::TreeWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TreeWindow)
{
    mRootWidget = this;
    while (mRootWidget->parentWidget())
        mRootWidget = mRootWidget->parentWidget();

    ui->setupUi(this);

    ui->tree->setStyleSheet(
                "QTreeView::item { "
                    "padding: 2px;"
                "}"
                );

    initPropertiesView();

    QList<QWidget*> widgets = mRootWidget->findChildren<QWidget*>();

    ui->tree->setRootIsDecorated(false);

    DomTreeItem *item = new DomTreeItem(ui->tree, mRootWidget);
    ui->tree->expandItem(item);
    ui->tree->resizeColumnToContents(0);


    connect(ui->tree, SIGNAL(itemSelectionChanged()), this, SLOT(updatePropertiesView()));
    item->setSelected(true);

}


/************************************************

 ************************************************/
TreeWindow::~TreeWindow()
{
    delete ui;
}


/************************************************

 ************************************************/
void TreeWindow::initPropertiesView()
{
    ui->propertiesView->viewport()->setAutoFillBackground(false);

    ui->propertiesView->setRowCount(4);
    ui->propertiesView->setColumnCount(2);

    QTableWidgetItem *item;
    item = new  QTableWidgetItem("Object name");
    ui->propertiesView->setItem(PROP_OBJECT_NAME, 0, item);
    ui->propertiesView->setItem(PROP_OBJECT_NAME, 1, new QTableWidgetItem());

    item = new  QTableWidgetItem("Class name");
    ui->propertiesView->setItem(PROP_CLASS_NAME, 0, item);
    ui->propertiesView->setItem(PROP_CLASS_NAME, 1, new QTableWidgetItem());

    item = new  QTableWidgetItem("Text");
    ui->propertiesView->setItem(PROP_TEXT, 0, item);
    ui->propertiesView->setItem(PROP_TEXT, 1, new QTableWidgetItem());

    item = new  QTableWidgetItem("Class hierarcy");
    ui->propertiesView->setItem(PROP_CLASS_HIERARCY, 0, item);
    ui->propertiesView->setItem(PROP_CLASS_HIERARCY, 1, new QTableWidgetItem());


}


/************************************************

 ************************************************/
void TreeWindow::updatePropertiesView()
{
    if (ui->tree->selectedItems().isEmpty())
    {
        clearPropertiesView();
        return;
    }

    QTreeWidgetItem *item = ui->tree->selectedItems().first();
    if (!item)
    {
        clearPropertiesView();
        return;
    }

    DomTreeItem *treeItem = static_cast<DomTreeItem*>(item);

    ui->propertiesView->item(PROP_OBJECT_NAME, 1)->setText(treeItem->widgetObjectName());
    ui->propertiesView->item(PROP_CLASS_NAME, 1)->setText(treeItem->widgetClassName());
    ui->propertiesView->item(PROP_TEXT, 1)->setText(treeItem->widgetText());
    ui->propertiesView->item(PROP_CLASS_HIERARCY, 1)->setText(treeItem->widgetClassHierarcy().join(" :: "));
}


/************************************************

 ************************************************/
void TreeWindow::clearPropertiesView()
{
    for (int i=0; i<ui->propertiesView->rowCount(); ++i)
        ui->propertiesView->item(i, 1)->setText("");
}








