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

#include "razorconfigdialog.h"
#include "ui_razorconfigdialog.h"

#include <qtxdg/xdgicon.h>

RazorConfigDialog::RazorConfigDialog(const QString& title, RazorSettings* settings, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::RazorConfigDialog),
    mSettings(settings),
    mCache(new RazorSettingsCache(settings))
{
    ui->setupUi(this);
    setWindowTitle(title);
    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), SLOT(dialogButtonsAction(QAbstractButton*)));
    ui->moduleList->setVisible(false);
}

void RazorConfigDialog::addPage(QWidget* page, const QString& name, const QString& iconName)
{
    new QListWidgetItem(XdgIcon::fromTheme(iconName, XdgIcon::fromTheme("application-x-executable")), name, ui->moduleList);
    ui->stackedWidget->addWidget(page);
    ui->moduleList->setVisible(true /*ui->stackedWidget->count() > 1*/);
    ui->moduleList->setCurrentRow(0);
    maxSize = QSize(qMax(page->geometry().width() + ui->moduleList->geometry().width(), maxSize.width()),
                    qMax(page->geometry().height() + ui->buttons->geometry().height(), maxSize.height()));
    resize(maxSize);
}

void RazorConfigDialog::closeEvent(QCloseEvent* event)
{
    mSettings->sync();
    emit save();
}

void RazorConfigDialog::dialogButtonsAction(QAbstractButton* button)
{
    QDialogButtonBox::ButtonRole role = ui->buttons->buttonRole(button);
    if (role == QDialogButtonBox::ResetRole)
    {
        mCache->loadToSettings();
        emit reset();
    }
    else
    {
        close();
    }
}

RazorConfigDialog::~RazorConfigDialog()
{
    delete ui;
    delete mCache;
}
