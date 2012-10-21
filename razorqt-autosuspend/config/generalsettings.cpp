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
#include <QtGui/qabstractbutton.h>

#include "generalsettings.h"
#include "ui_generalsettings.h"

GeneralSettings::GeneralSettings(RazorSettings *settings, QWidget *parent) :
    QWidget(parent),
    mUi(new Ui::GeneralSettings),
    mLoading(false)
{
    this->mSettings = settings;
    mUi->setupUi(this);

    connect(mUi->showTrayIconCheckBox, SIGNAL(stateChanged(int)), this, SLOT(saveSettings()));
    connect(mUi->useThemeStatusIconsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(saveSettings()));
}

GeneralSettings::~GeneralSettings()
{
    delete mUi;
}

void GeneralSettings::saveSettings()
{
    if (mLoading) return; // If we come heare because checkboxes changed state during loading
                         // we don't wan't to save

    mSettings->setValue(SHOWTRAYICON_KEY, mUi->showTrayIconCheckBox->isChecked());
    mSettings->setValue(USETHEMEICONS_KEY, mUi->useThemeStatusIconsCheckBox->isChecked());
    mUi->useThemeStatusIconsCheckBox->setEnabled(mUi->showTrayIconCheckBox->isChecked());
}

void GeneralSettings::loadSettings()
{
    mLoading = true; 
    mUi->showTrayIconCheckBox->setChecked(mSettings->value(SHOWTRAYICON_KEY, true).toBool());
    mUi->useThemeStatusIconsCheckBox->setChecked(mSettings->value(USETHEMEICONS_KEY, true).toBool());
    mUi->useThemeStatusIconsCheckBox->setEnabled(mUi->showTrayIconCheckBox->isChecked());
    mLoading = false;
}
