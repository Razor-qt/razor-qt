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
#include "generalsettings.h"
#include "ui_generalsettings.h"

GeneralSettings::GeneralSettings(RazorSettings *settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeneralSettings)
{
    this->m_Settings = settings;
    ui->setupUi(this);

    connect(ui->showTrayIconcheckBox, SIGNAL(stateChanged(int)), this, SLOT(saveSettings()));
}

GeneralSettings::~GeneralSettings()
{
    delete ui;
}

void GeneralSettings::saveSettings()
{
    m_Settings->setValue(SHOWTRAYICON_KEY, ui->showTrayIconcheckBox->isChecked());
}

void GeneralSettings::loadSettings()
{
    ui->showTrayIconcheckBox->setChecked(m_Settings->value(SHOWTRAYICON_KEY, true).toBool());
}
