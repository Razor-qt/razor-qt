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
#include "lidsettings.h"
#include "ui_lidsettings.h"
#include "constants.h"

LidSettings::LidSettings(RazorSettings *settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LidSettings),
    loading(false)
{
    mSettings = settings;
    ui->setupUi(this);
    connect(ui->actionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(saveAction()));
}

LidSettings::~LidSettings()
{
    delete ui;
}


void LidSettings::loadSettings()
{
    loading = true;
    ui->actionComboBox->clear();
    ui->actionComboBox->addItem(tr("Nothing"), NOTHING);
    ui->actionComboBox->addItem(tr("Sleep"), SLEEP);
    ui->actionComboBox->addItem(tr("Hibernate"), HIBERNATE);

    for (int index = 0; index < ui->actionComboBox->count(); index++)
    {
        if (mSettings->value(LIDCLOSEDACTION_KEY, 0) == ui->actionComboBox->itemData(index).toInt())
        {
            ui->actionComboBox->setCurrentIndex(index);
            break;
        }
    }
    loading = false;
}

void LidSettings::saveAction()
{
    if (! loading)
    {
        mSettings->setValue(LIDCLOSEDACTION_KEY, ui->actionComboBox->itemData(ui->actionComboBox->currentIndex()).toInt());
    }
}
