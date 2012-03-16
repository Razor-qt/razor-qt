/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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

#include "basicsettings.h"
#include "ui_basicsettings.h"

BasicSettings::BasicSettings(RazorSettings* settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BasicSettings),
    mSettings(settings)
{
    ui->setupUi(this);

    ui->desktopTypeComboBox->addItem("Razor Desktop", "razor");
    ui->desktopTypeComboBox->addItem("Window Manager Native", "wm_native");

    restoreSettings();

    connect(ui->desktopTypeComboBox, SIGNAL(activated(int)),
            this, SLOT(desktopTypeComboBox_currentIndexChanged(int)));

    connect(ui->singleclickButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->doubleclickButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->wheelDesktopCheckBox, SIGNAL(toggled(bool)), this, SLOT(save()));
    connect(ui->useDifferentWallpapersCheckBox, SIGNAL(clicked()), this, SLOT(save()));
}

BasicSettings::~BasicSettings()
{
    delete ui;
}

void BasicSettings::restoreSettings()
{
    QString desktopType = mSettings->value("desktop", "razor").toString();

    int ix = ui->desktopTypeComboBox->findData(desktopType);
    if (ix == -1)
        ix = 0;
    ui->desktopTypeComboBox->setCurrentIndex(ix);
    desktopTypeComboBox_currentIndexChanged(ix);

    QString clickType = mSettings->value("icon-launch-mode", "singleclick").toString();
    if (clickType == "singleclick")
        ui->singleclickButton->setChecked(true);
    else
        ui->doubleclickButton->setChecked(true);

    mSettings->beginGroup("razor");
    ui->wheelDesktopCheckBox->setChecked(mSettings->value("mouse_wheel_desktop_switch", false).toBool());
    ui->useDifferentWallpapersCheckBox->setChecked(mSettings->value("use_different_wallpapers", false).toBool());
    mSettings->endGroup();
}

void BasicSettings::save()
{
    mSettings->setValue("icon-launch-mode", ui->singleclickButton->isChecked() ? "singleclick" : "doubleclick");

    mSettings->beginGroup("razor");
    mSettings->setValue("mouse_wheel_desktop_switch", ui->wheelDesktopCheckBox->isChecked());
    mSettings->setValue("use_different_wallpapers", ui->useDifferentWallpapersCheckBox->isChecked());
    mSettings->endGroup();

    emit needRestart();
}

void BasicSettings::desktopTypeComboBox_currentIndexChanged(int ix)
{
    QString t = ui->desktopTypeComboBox->itemData(ix).toString();
    if (t == "razor")
    {
        ui->desktopTypeLabels->setText(tr("<b>Razor Desktop</b><br>"
                                      "Desktop is fully controlled by Razor. <b>Configuration is available in the desktop context menu.</b>"
                                     )
                                  );
    }
    else
    {
        ui->desktopTypeLabels->setText(tr("<b>Window Manager Native Desktop</b><br>"
                                      "Desktop is handled by used window manager (including WM's own menus etc.). Only few things can be configured here."
                                     )
                                  );
    }
    mSettings->setValue("desktop", ui->desktopTypeComboBox->itemData(ui->desktopTypeComboBox->currentIndex()).toString());
    emit needRestart();
}
