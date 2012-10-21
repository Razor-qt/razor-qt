/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
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
#include <QtGui/qabstractbutton.h>

#include "generalsettings.h"
#include "ui_generalsettings.h"

GeneralSettings::GeneralSettings(RazorSettings *settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeneralSettings),
    loading(false)
{
    this->m_Settings = settings;
    ui->setupUi(this);

    connect(ui->showTrayIconCheckBox, SIGNAL(stateChanged(int)), this, SLOT(saveSettings()));
    connect(ui->useThemeStatusIconsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(saveSettings()));
}

GeneralSettings::~GeneralSettings()
{
    delete ui;
}

void GeneralSettings::saveSettings()
{
    if (loading) return; // If we come heare because checkboxes changed state during loading
                         // we don't wan't to save

    m_Settings->setValue(SHOWTRAYICON_KEY, ui->showTrayIconCheckBox->isChecked());
    m_Settings->setValue(USETHEMEICONS_KEY, ui->useThemeStatusIconsCheckBox->isChecked());
    ui->useThemeStatusIconsCheckBox->setEnabled(ui->showTrayIconCheckBox->isChecked());
}

void GeneralSettings::loadSettings()
{
    loading = true; 
    ui->showTrayIconCheckBox->setChecked(m_Settings->value(SHOWTRAYICON_KEY, true).toBool());
    ui->useThemeStatusIconsCheckBox->setChecked(m_Settings->value(USETHEMEICONS_KEY, true).toBool());
    ui->useThemeStatusIconsCheckBox->setEnabled(ui->showTrayIconCheckBox->isChecked());
    loading = false;
}
