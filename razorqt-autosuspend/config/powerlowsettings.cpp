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
#include "powerlowsettings.h"
#include "ui_powerlowsettings.h"
#include "constants.h"

PowerLowSettings::PowerLowSettings(RazorSettings *settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PowerLowSettings), loading(false)
{
    m_Settings = settings;
    ui->setupUi(this);

    connect(ui->actionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(saveAction()));
    connect(ui->warningSpinBox, SIGNAL(valueChanged(int)), this, SLOT(saveWarningTime()));
}

PowerLowSettings::~PowerLowSettings()
{
    delete ui;
}

void PowerLowSettings::loadSettings()
{
    loading = true;
    ui->actionComboBox->clear();
    ui->actionComboBox->addItem(tr("Nothing"), NOTHING);
    ui->actionComboBox->addItem(tr("Sleep"), SLEEP);
    ui->actionComboBox->addItem(tr("Hibernate"), HIBERNATE);

    for (int index = 0; index < ui->actionComboBox->count(); index++)
    {
        if (m_Settings->value(POWERLOWACTION_KEY, 0) == ui->actionComboBox->itemData(index).toInt())
        {
            ui->actionComboBox->setCurrentIndex(index);
            break;
        }
    }

    ui->warningSpinBox->setValue(m_Settings->value(POWERLOWWARNING_KEY, 30).toInt());
    loading = false;
}

void PowerLowSettings::saveAction()
{
    if (! loading)
    {
        m_Settings->setValue(POWERLOWACTION_KEY, ui->actionComboBox->itemData(ui->actionComboBox->currentIndex()).toInt());
    }
}

void PowerLowSettings::saveWarningTime()
{
    if (! loading)
    {
        m_Settings->setValue(POWERLOWWARNING_KEY, ui->warningSpinBox->value());
    }
}

