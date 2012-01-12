/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright (C) 2011  Alec Moskvin <alecm@gmx.com>
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

#include <QtGui/QFileDialog>

#include <qtxdg/xdgautostart.h>

#include "autostartedit.h"
#include "ui_autostartedit.h"

AutoStartEdit::AutoStartEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoStartEdit)
{
    ui->setupUi(this);
    connect(ui->browseButton, SIGNAL(clicked()), SLOT(browse()));
}

XdgDesktopFile* AutoStartEdit::createXdgFile()
{
    XdgDesktopFile* file = NULL;
    if (exec() == QDialog::Accepted)
    {
        file = new XdgDesktopFile(XdgDesktopFile::ApplicationType,
                                                  ui->nameEdit->text(),
                                                  ui->commandEdit->text());
    }
    return file;
}

bool AutoStartEdit::editXdgFile(XdgDesktopFile* file)
{
    QString command;
    if (file->contains("Exec"))
        command = file->value("Exec").toString();
    ui->nameEdit->setText(file->name());
    ui->commandEdit->setText(command);
    if (exec() == QDialog::Accepted)
    {
        file->setLocalizedValue("Name", ui->nameEdit->text());
        file->setValue("Exec", ui->commandEdit->text());
        return true;
    }
    return false;
}

void AutoStartEdit::browse()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Select Application"), "/usr/bin/");
    if (!filePath.isEmpty())
        ui->commandEdit->setText(filePath);
}

AutoStartEdit::~AutoStartEdit()
{
    delete ui;
}
