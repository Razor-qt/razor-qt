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

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::Settings), m_Settings("razor-autosuspend"), m_RollbackPoint(m_Settings) {
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);

    loadSettings();

    connect(ui->lidClosedActionComboBox, SIGNAL(activated(int)), this, SLOT(saveSettings()));
    connect(ui->powerLowActionComboBox, SIGNAL(activated(int)), this, SLOT(saveSettings()));
    connect(ui->powerLowWarningSpinBox, SIGNAL(valueChanged(int)), this, SLOT(saveSettings()));

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));

    m_RollbackPoint.loadFromSettings();
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::dialogButtonsAction(QAbstractButton *btn)
{
    if (ui->buttonBox->buttonRole(btn) == QDialogButtonBox::ResetRole)
    {
        m_RollbackPoint.loadToSettings();
        loadSettings();
    }
    else
    {
        close();
    }
}

void SettingsDialog::loadSettings()
{
    fillInActions(ui->lidClosedActionComboBox, m_Settings.value(LIDCLOSEDACTION_KEY).toInt());
    fillInActions(ui->powerLowActionComboBox, m_Settings.value(POWERLOWACTION_KEY).toInt());
    ui->powerLowWarningSpinBox->setValue(m_Settings.value(POWERLOWWARNING_KEY, 30).toInt());
}

void SettingsDialog::fillInActions(QComboBox *comboBox, int selectedData)
{
    comboBox->clear();
    comboBox->addItem(tr("Nothing"), NOTHING);
    comboBox->addItem(tr("Sleep"), SLEEP);
    comboBox->addItem(tr("Hibernate"), HIBERNATE);
    for (int index = 0; index < comboBox->count(); index++)
    {
        if (selectedData == comboBox->itemData(index).toInt())
        {
            comboBox->setCurrentIndex(index);
            break;
        }
    }
}

void SettingsDialog::saveSettings()
{
    m_Settings.setValue(LIDCLOSEDACTION_KEY, ui->lidClosedActionComboBox->itemData(ui->lidClosedActionComboBox->currentIndex()).toInt());
    m_Settings.setValue(POWERLOWACTION_KEY, ui->powerLowActionComboBox->itemData(ui->powerLowActionComboBox->currentIndex()).toInt());
    m_Settings.setValue(POWERLOWWARNING_KEY, ui->powerLowWarningSpinBox->value());
}
