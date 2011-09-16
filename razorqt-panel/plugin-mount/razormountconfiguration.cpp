/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
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


#include "razormountconfiguration.h"
#include "ui_razormountconfiguration.h"

#include <QComboBox>
#include <QDebug>

#define ACT_SHOW_MENU "showMenu"
#define ACT_SHOW_INFO "showInfo"
#define ACT_NOTHING   "nothing"

RazorMountConfiguration::RazorMountConfiguration(QSettings &settings, QWidget *parent) :
    RazorPanelPluginConfigDialog(settings, parent),
    ui(new Ui::RazorMountConfiguration)
{
    ui->setupUi(this);
    ui->devAddedCombo->addItem(tr("Popup menu"), ACT_SHOW_MENU);
    ui->devAddedCombo->addItem(tr("Show info"),  ACT_SHOW_INFO);
    ui->devAddedCombo->addItem(tr("Do nothing"), ACT_NOTHING);

    loadSettings();
    connect(ui->devAddedCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(devAddedChanged(int)));
    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));
}

RazorMountConfiguration::~RazorMountConfiguration()
{
    delete ui;
}


void RazorMountConfiguration::loadSettings()
{
    setComboboxIndexByData(ui->devAddedCombo, settings().value("newDeviceAction", ACT_SHOW_INFO), 1);
}


void RazorMountConfiguration::devAddedChanged(int index)
{
    QString s = ui->devAddedCombo->itemData(index).toString();
    settings().setValue("newDeviceAction", s);
}
