/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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


#include "razorkbindicatorconfiguration.h"
#include "ui_razorkbindicatorconfiguration.h"


RazorKbIndicatorConfiguration::RazorKbIndicatorConfiguration(QSettings &settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RazorKbIndicatorConfiguration),
    mSettings(settings),
    oldSettings(settings),
    lockSaving(false)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("KbIndicatorConfigurationWindow");
    ui->setupUi(this);

    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));

    connect(ui->capsLockRB, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->numLockRB, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->scrollLockRB, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->advancedRB, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->bitSB, SIGNAL(valueChanged(int)), this, SLOT(saveSettings()));
    connect(ui->textLE, SIGNAL(textChanged(QString)), this, SLOT(saveSettings()));

    loadSettings();
}

RazorKbIndicatorConfiguration::~RazorKbIndicatorConfiguration()
{
    delete ui;
}

void RazorKbIndicatorConfiguration::loadSettings()
{
    lockSaving = true;

    ui->bitSB->setValue( mSettings.value("bit", 0).toInt() );
    ui->textLE->setText( mSettings.value("text", QString("C")).toString() );

    if ((ui->bitSB->value() == 0) && (ui->textLE->text() == QString("C")))
        ui->capsLockRB->setChecked(true);
    else if ((ui->bitSB->value() == 1) && (ui->textLE->text() == QString("1")))
        ui->numLockRB->setChecked(true);
    else if ((ui->bitSB->value() == 2) && (ui->textLE->text() == QString("S")))
        ui->scrollLockRB->setChecked(true);
    else
        ui->advancedRB->setChecked(true);

    ui->advancedW->setVisible(ui->advancedRB->isChecked());

    lockSaving = false;
}

void RazorKbIndicatorConfiguration::saveSettings()
{
    if (lockSaving)
        return;

    if (ui->capsLockRB->isChecked())
    {
        ui->bitSB->setValue(0);
        ui->textLE->setText(QString("C"));
    }
    else if (ui->numLockRB->isChecked())
    {
        ui->bitSB->setValue(1);
        ui->textLE->setText(QString("1"));
    }
    else if (ui->scrollLockRB->isChecked())
    {
        ui->bitSB->setValue(2);
        ui->textLE->setText(QString("S"));
    }

    mSettings.setValue("bit", ui->bitSB->value());
    mSettings.setValue("text", ui->textLE->text());
}

void RazorKbIndicatorConfiguration::dialogButtonsAction(QAbstractButton *btn)
{
    if (ui->buttons->buttonRole(btn) == QDialogButtonBox::ResetRole)
    {
        oldSettings.loadToSettings();
        loadSettings();
    }
    else
        close();
}
