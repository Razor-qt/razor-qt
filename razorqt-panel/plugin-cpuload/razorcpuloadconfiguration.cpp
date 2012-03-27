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

RazorCpuLoadConfiguration::RazorCpuLoadConfiguration(QSettings &settings, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::RazorCpuLoadConfiguration),
	mSettings(settings),
	mOldSettings(settings)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setObjectName("CpuLoadConfigurationWindow");
	ui->setupUi(this);

	connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));

	loadSettings();

	connect(ui->showTextCB, SIGNAL(toggled(bool)), this, SLOT(showTextChanged(bool)));
}

RazorCpuLoadConfiguration::~RazorCpuLoadConfiguration()
{
	delete ui;
}

void RazorCpuLoadConfiguration::loadSettings()
{
	ui->showTextCB->setChecked(mSettings.value("showText", false).toBool());

//	QString menuFile = mSettings.value("menu_file", "").toString();
//	if (menuFile.isEmpty())
//	{
//		menuFile = XdgMenu::getMenuFileName();
//	}
//	ui->menuFilePathLE->setText(menuFile);
//	ui->shortcutEd->setKeySequence(mSettings.value("shortcut", "Alt+F1").toString());
}

void RazorCpuLoadConfiguration::showTextChanged(bool value)
{
	mSettings.setValue("showText", value);
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

