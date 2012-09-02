/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright (C) 2012  Alec Moskvin <alecm@gmx.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "autostartpage.h"
#include "ui_autostartpage.h"

#include "autostartedit.h"

#include <QtGui/QMessageBox>

AutoStartPage::AutoStartPage(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AutoStartPage)
{
    ui->setupUi(this);

    connect(ui->addButton, SIGNAL(clicked()), SLOT(addButton_clicked()));
    connect(ui->editButton, SIGNAL(clicked()), SLOT(editButton_clicked()));
    connect(ui->deleteButton, SIGNAL(clicked()), SLOT(deleteButton_clicked()));

    restoreSettings();
}

AutoStartPage::~AutoStartPage()
{
    delete ui;
}

void AutoStartPage::restoreSettings()
{
    QAbstractItemModel* oldModel = ui->autoStartView->model();
    mXdgAutoStartModel = new AutoStartItemModel(ui->autoStartView);
    ui->autoStartView->setModel(mXdgAutoStartModel);
    delete oldModel;
    ui->autoStartView->setExpanded(mXdgAutoStartModel->index(0, 0), true);
    ui->autoStartView->setExpanded(mXdgAutoStartModel->index(1, 0), true);
    updateButtons();
    connect(mXdgAutoStartModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(updateButtons()));
    connect(ui->autoStartView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(selectionChanged(QModelIndex)));
}

void AutoStartPage::save()
{
    mXdgAutoStartModel->writeChanges();
}

void AutoStartPage::addButton_clicked()
{
    AutoStartEdit edit(QString(), QString(), false);
    bool success = false;
    while (!success && edit.exec() == QDialog::Accepted)
    {
        QModelIndex index = ui->autoStartView->selectionModel()->currentIndex();
        XdgDesktopFile file(XdgDesktopFile::ApplicationType, edit.name(), edit.command());
        if (edit.needTray())
            file.setValue("X-Razor-Need-Tray", true);
        if (mXdgAutoStartModel->setEntry(index, file))
            success = true;
        else
            QMessageBox::critical(this, tr("Error"), tr("File '%1' already exists!").arg(file.fileName()));
    }
}

void AutoStartPage::editButton_clicked()
{
    QModelIndex index = ui->autoStartView->selectionModel()->currentIndex();
    XdgDesktopFile file = mXdgAutoStartModel->desktopFile(index);
    AutoStartEdit edit(file.name(), file.value("Exec").toString(), file.contains("X-Razor-Need-Tray"));
    if (edit.exec() == QDialog::Accepted)
    {
        file.setLocalizedValue("Name", edit.name());
        file.setValue("Exec", edit.command());
        if (edit.needTray())
            file.setValue("X-Razor-Need-Tray", true);
        else
            file.removeEntry("X-Razor-Need-Tray");

        mXdgAutoStartModel->setEntry(index, file, true);
    }
}

void AutoStartPage::deleteButton_clicked()
{
    QModelIndex index = ui->autoStartView->selectionModel()->currentIndex();
    mXdgAutoStartModel->removeRow(index.row(), index.parent());
}

void AutoStartPage::selectionChanged(QModelIndex curr)
{
    AutoStartItemModel::ActiveButtons active = mXdgAutoStartModel->activeButtons(curr);
    ui->addButton->setEnabled(active & AutoStartItemModel::AddButton);
    ui->editButton->setEnabled(active & AutoStartItemModel::EditButton);
    ui->deleteButton->setEnabled(active & AutoStartItemModel::DeleteButton);
}

void AutoStartPage::updateButtons()
{
    selectionChanged(ui->autoStartView->selectionModel()->currentIndex());
}
