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


#include "razornetworkmonitorconfiguration.h"
#include "ui_razornetworkmonitorconfiguration.h"

RazorNetworkMonitorConfiguration::RazorNetworkMonitorConfiguration(QSettings &settings, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::RazorNetworkMonitorConfiguration),
	mSettings(settings),
	mOldSettings(settings)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setObjectName("NetworkMonitorConfigurationWindow");
	ui->setupUi(this);

	connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));

	loadSettings();
}

RazorNetworkMonitorConfiguration::~RazorNetworkMonitorConfiguration()
{
	delete ui;
}

void RazorNetworkMonitorConfiguration::loadSettings()
{
	ui->iconCB->setCurrentIndex( mSettings.value("icon", 1).toInt() );

	// TODO: use iface list from libstatgrab with editable combobox
	ui->interfaceLE->setText(mSettings.value("interface", "eth0").toString());
}

void RazorNetworkMonitorConfiguration::dialogButtonsAction(QAbstractButton *btn)
{
	if (ui->buttons->buttonRole(btn) == QDialogButtonBox::ResetRole)
	{
		mOldSettings.loadToSettings();
		loadSettings();
	}
	else
	{
		mSettings.setValue("icon", ui->iconCB->currentIndex());
		mSettings.setValue("interface", ui->interfaceLE->text());

		close();
	}
}

