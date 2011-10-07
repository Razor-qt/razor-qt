/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Maciej Płaza <plaza.maciej@gmail.com>
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

#include "razortaskbarconfiguration.h"
#include "ui_razortaskbarconfiguration.h"

RazorTaskbarConfiguration::RazorTaskbarConfiguration(QSettings &settings, QWidget *parent):
    QDialog(parent),
    ui(new Ui::RazorTaskbarConfiguration),
    mSettings(settings),
    oldSettings(settings)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("TaskbarConfigurationWindow");
    ui->setupUi(this);

    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));

    ui->buttonStyleCB->addItem(tr("Icon and text"), "IconText");
    ui->buttonStyleCB->addItem(tr("Only icon"), "Icon");
    ui->buttonStyleCB->addItem(tr("Only text"), "Text");

    loadSettings();
    /* We use clicked() and activated(int) because these signals aren't emitting after programmaticaly
        change of state */
    connect(ui->fAllDesktopsCB, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->fCurrentDesktopRB, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->buttonStyleCB, SIGNAL(activated(int)), this, SLOT(updateControls(int)));
    connect(ui->buttonStyleCB, SIGNAL(activated(int)), this, SLOT(saveSettings()));
    connect(ui->maxWidthSB, SIGNAL(valueChanged(int)), this, SLOT(saveSettings()));
}

RazorTaskbarConfiguration::~RazorTaskbarConfiguration()
{
    delete ui;
}

void RazorTaskbarConfiguration::loadSettings()
{
    if (mSettings.value("showOnlyCurrentDesktopTasks", false).toBool() == true)
    {
        ui->fCurrentDesktopRB->setChecked(true);
    }
    else
    {
        ui->fAllDesktopsCB->setChecked(true);
    }

    ui->buttonStyleCB->setCurrentIndex(ui->buttonStyleCB->findData(mSettings.value("buttonStyle", "IconText")));
    updateControls(ui->buttonStyleCB->currentIndex());

    /* Keep maxWidth loading at the end of this method to prevent errors */
    ui->maxWidthSB->setValue(mSettings.value("maxWidth", 400).toInt());
}

void RazorTaskbarConfiguration::saveSettings()
{
    mSettings.setValue("showOnlyCurrentDesktopTasks", ui->fCurrentDesktopRB->isChecked());
    mSettings.setValue("buttonStyle", ui->buttonStyleCB->itemData(ui->buttonStyleCB->currentIndex()));
    mSettings.setValue("maxWidth", ui->maxWidthSB->value());
}

void RazorTaskbarConfiguration::updateControls(int index)
{
    if (ui->buttonStyleCB->itemData(index) == "Icon")
    {
        ui->maxWidthSB->setEnabled(false);
        ui->maxWidthL->setEnabled(false);
    }
    else
    {
        ui->maxWidthSB->setEnabled(true);
        ui->maxWidthL->setEnabled(true);
    }
}

void RazorTaskbarConfiguration::dialogButtonsAction(QAbstractButton *btn)
{
    if (ui->buttons->buttonRole(btn) == QDialogButtonBox::ResetRole)
    {
        /* We have to disable signals for maxWidthSB to prevent errors. Otherwise not all data
          could be restored */
        ui->maxWidthSB->blockSignals(true);
        oldSettings.loadToSettings();
        loadSettings();
        ui->maxWidthSB->blockSignals(false);
    }
    else
    {
        close();
    }
}
