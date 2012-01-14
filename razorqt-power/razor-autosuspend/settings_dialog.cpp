/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Christian Surlykke <christian@surlykke.dk>
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

#include "settings_dialog.h"
#include "ui_settings_dialog.h"
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::Settings), settings("razor-autosuspend") {
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    lidClosedGroup.addButton(ui->lidClosedNothingButton, NOTHING);
    lidClosedGroup.addButton(ui->lidClosedSleepButton, SLEEP);
    lidClosedGroup.addButton(ui->lidClosedHibernateButton, HIBERNATE);
    powerLowGroup.addButton(ui->powerLowNothingButton, NOTHING);
    powerLowGroup.addButton(ui->powerLowSleepButton, SLEEP);
    powerLowGroup.addButton(ui->powerLowHibernateButton, HIBERNATE);
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::accept() {
    settings.setValue(LIDCLOSEDACTION_KEY , lidClosedGroup.checkedId());
    settings.setValue(POWERLOWACTION_KEY, powerLowGroup.checkedId());
    QDialog::accept();
}

void SettingsDialog::showEvent(QShowEvent *) {
    QAbstractButton *button;
    button = lidClosedGroup.button(settings.value(LIDCLOSEDACTION_KEY).toInt());
    if (button != 0) {
        button->setChecked(true);
    }

    button = powerLowGroup.button(settings.value(POWERLOWACTION_KEY).toInt());
    if (button != 0) {
        button->setChecked(true);
    }
}
