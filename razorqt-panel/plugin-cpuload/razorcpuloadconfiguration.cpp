/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
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

#include "razorcpuloadconfiguration.h"
#include "ui_razorcpuloadconfiguration.h"

#define BAR_ORIENT_BOTTOMUP "bottomUp"
#define BAR_ORIENT_TOPDOWN "topDown"
#define BAR_ORIENT_LEFTRIGHT "leftRight"
#define BAR_ORIENT_RIGHTLEFT "rightLeft"

RazorCpuLoadConfiguration::RazorCpuLoadConfiguration(QSettings *settings, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::RazorCpuLoadConfiguration),
	mSettings(settings),
	mOldSettings(settings)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setObjectName("CpuLoadConfigurationWindow");
	ui->setupUi(this);

    fillBarOrientations();

	connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(dialogButtonsAction(QAbstractButton*)));

	loadSettings();

	connect(ui->showTextCB, SIGNAL(toggled(bool)),
            this, SLOT(showTextChanged(bool)));
    connect(ui->updateIntervalSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(updateIntervalChanged(double)));
    connect(ui->barOrientationCOB, SIGNAL(currentIndexChanged(int)),
            this, SLOT(barOrientationChanged(int)));
}

RazorCpuLoadConfiguration::~RazorCpuLoadConfiguration()
{
	delete ui;
}

void RazorCpuLoadConfiguration::fillBarOrientations()
{
    ui->barOrientationCOB->addItem(trUtf8("Bottom up"), BAR_ORIENT_BOTTOMUP);
    ui->barOrientationCOB->addItem(trUtf8("Top down"), BAR_ORIENT_TOPDOWN);
    ui->barOrientationCOB->addItem(trUtf8("Left to right"), BAR_ORIENT_LEFTRIGHT);
    ui->barOrientationCOB->addItem(trUtf8("Right to left"), BAR_ORIENT_RIGHTLEFT);
}

void RazorCpuLoadConfiguration::loadSettings()
{
    ui->showTextCB->setChecked(mSettings->value("showText", false).toBool());
    ui->updateIntervalSpinBox->setValue(mSettings->value("updateInterval", 1000).toInt() / 1000.0);

    int boIndex = ui->barOrientationCOB->findData(
            mSettings->value("barOrientation", BAR_ORIENT_BOTTOMUP));
    boIndex = (boIndex < 0) ? 1 : boIndex;
    ui->barOrientationCOB->setCurrentIndex(boIndex);

//	QString menuFile = mSettings->value("menu_file", "").toString();
//	if (menuFile.isEmpty())
//	{
//		menuFile = XdgMenu::getMenuFileName();
//	}
//	ui->menuFilePathLE->setText(menuFile);
//	ui->shortcutEd->setKeySequence(mSettings->value("shortcut", "Alt+F1").toString());
}

void RazorCpuLoadConfiguration::showTextChanged(bool value)
{
    mSettings->setValue("showText", value);
}


void RazorCpuLoadConfiguration::updateIntervalChanged(double value)
{
    mSettings->setValue("updateInterval", value*1000);
}

void RazorCpuLoadConfiguration::barOrientationChanged(int index)
{
    mSettings->setValue("barOrientation", ui->barOrientationCOB->itemData(index).toString());
}

void RazorCpuLoadConfiguration::dialogButtonsAction(QAbstractButton *btn)
{
	if (ui->buttons->buttonRole(btn) == QDialogButtonBox::ResetRole)
	{
		mOldSettings.loadToSettings();
		loadSettings();
	}
	else
	{
		close();
	}
}

