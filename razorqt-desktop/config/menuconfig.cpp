/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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

#include <QtGui/QFileDialog>

#include "menuconfig.h"
#include "ui_menuconfig.h"

MenuConfig::MenuConfig(RazorSettings* settings, QWidget *parent):
    QWidget(parent),
    ui(new Ui::MenuConfig),
    mSettings(settings)
{
    ui->setupUi(this);
    restoreSettings();
    connect(ui->chooseMenuFilePB, SIGNAL(clicked()), this, SLOT(chooseMenuFile()));
}

MenuConfig::~MenuConfig()
{
    delete ui;
}

void MenuConfig::restoreSettings()
{
    mSettings->beginGroup("razor");
    ui->menuFilePathLE->setText(mSettings->value("menu_file").toString());
    mSettings->endGroup();
}

void MenuConfig::chooseMenuFile()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Choose menu file"), QDir::homePath(), tr("Menu files (*.menu)"));
    if (!path.isEmpty())
    {
        ui->menuFilePathLE->setText(path);
        mSettings->setValue("menu_file", ui->menuFilePathLE->text());
        emit needRestart();
    }
}
