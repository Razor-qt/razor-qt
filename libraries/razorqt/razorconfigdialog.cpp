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
    mSettings(settings),
    mCache(new RazorSettingsCache(settings)),
    ui(new Ui::RazorConfigDialog)
{
    ui->setupUi(this);
    setWindowTitle(title);
    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), SLOT(dialogButtonsAction(QAbstractButton*)));
    ui->moduleList->setVisible(false);
    connect(RazorSettings::globalSettings(), SIGNAL(settingsChanged()), this, SLOT(updateIcons()));
}

void RazorConfigDialog::addPage(QWidget* page, const QString& name, const QString& iconName)
{
    addPage(page, name, QStringList() << iconName);
}

void RazorConfigDialog::addPage(QWidget* page, const QString& name, const QStringList& iconNames)
{
    QStringList icons = QStringList(iconNames) << "application-x-executable";
    new QListWidgetItem(XdgIcon::fromTheme(icons), name, ui->moduleList);
    mIcons.append(icons);
    ui->stackedWidget->addWidget(page);
    ui->moduleList->setVisible(true /*ui->stackedWidget->count() > 1*/);
    ui->moduleList->setCurrentRow(0);
    maxSize = QSize(qMax(page->geometry().width() + ui->moduleList->geometry().width(), maxSize.width()),
                    qMax(page->geometry().height() + ui->buttons->geometry().height(), maxSize.height()));
    resize(maxSize);
}

void RazorConfigDialog::closeEvent(QCloseEvent* event)
{
    emit save();
    mSettings->sync();
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

void RazorConfigDialog::updateIcons()
{
    for (int ix = 0; ix < mIcons.size(); ix++)
        ui->moduleList->item(ix)->setIcon(XdgIcon::fromTheme(mIcons.at(ix)));
    update();
}

RazorConfigDialog::~RazorConfigDialog()
{
    delete ui;
    delete mCache;
}
