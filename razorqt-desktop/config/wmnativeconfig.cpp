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

#include "wmnativeconfig.h"
#include "ui_wmnativeconfig.h"

#include <QtGui/QFileDialog>

WMNativeConfig::WMNativeConfig(RazorSettings *settings, QWidget *parent):
    QWidget(parent),
    ui(new Ui::WMNativeConfig),
    mSettings(settings)
{
    ui->setupUi(this);
    restoreSettings();
    connect(ui->nativeWallpaperButton, SIGNAL(clicked()), SLOT(nativeWallpaperButton_clicked()));
    connect(ui->nativeWallpaperEdit, SIGNAL(editingFinished()), this, SLOT(save()));
    connect(ui->nativeIconsCheckBox, SIGNAL(clicked()), this, SLOT(save()));
}

WMNativeConfig::~WMNativeConfig()
{
    delete ui;
}

void WMNativeConfig::restoreSettings()
{
    mSettings->beginGroup("wm_native");
    QString wmWallpaper = mSettings->value("wallpaper").toString();
    ui->nativeWallpaperEdit->setText(wmWallpaper);
    bool wmIcons = mSettings->value("icons", true).toBool();
    ui->nativeIconsCheckBox->setChecked(wmIcons);
    mSettings->endGroup();
    ui->nativeWallpaperEdit->setText(wmWallpaper);
    ui->nativeIconsCheckBox->setChecked(wmIcons);
}

void WMNativeConfig::save()
{
    mSettings->beginGroup("wm_native");
    mSettings->setValue("wallpaper", ui->nativeWallpaperEdit->text());
    mSettings->setValue("icons", ui->nativeIconsCheckBox->isChecked());
    mSettings->endGroup();
    emit needRestart();
}

void WMNativeConfig::nativeWallpaperButton_clicked()
{
    QString fname = QFileDialog::getOpenFileName(this, tr("Select Wallpaper Image"),
                                                 QDir::homePath(), tr("Images (*.png *.xpm *.jpg)"));
    if (!fname.isNull())
        ui->nativeWallpaperEdit->setText(fname);
}
