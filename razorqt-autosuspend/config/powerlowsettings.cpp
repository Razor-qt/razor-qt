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
    mUi(new Ui::PowerLowSettings), mLoading(false)
{
    mSettings = settings;
    mUi->setupUi(this);

    connect(mUi->actionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(saveSettings()));
    connect(mUi->warningSpinBox, SIGNAL(valueChanged(int)), this, SLOT(saveSettings()));
    connect(mUi->levelSpinBox, SIGNAL(valueChanged(int)), this, SLOT(saveSettings()));
}

PowerLowSettings::~PowerLowSettings()
{
    delete mUi;
}

void PowerLowSettings::loadSettings()
{
    mLoading = true;
    mUi->actionComboBox->clear();
    mUi->actionComboBox->addItem(tr("Nothing"), NOTHING);
    mUi->actionComboBox->addItem(tr("Sleep"), SLEEP);
    mUi->actionComboBox->addItem(tr("Hibernate"), HIBERNATE);
    mUi->actionComboBox->addItem(tr("Shutdown"), POWEROFF);

    for (int index = 0; index < mUi->actionComboBox->count(); index++)
    {
        if (mSettings->value(POWERLOWACTION_KEY, 0) == mUi->actionComboBox->itemData(index).toInt())
        {
            mUi->actionComboBox->setCurrentIndex(index);
            break;
        }
    }

    mUi->warningSpinBox->setValue(mSettings->value(POWERLOWWARNING_KEY, 30).toInt());
    mUi->levelSpinBox->setValue(mSettings->value(POWERLOWLEVEL_KEY, 15).toInt());
    mLoading = false;
}

void PowerLowSettings::saveSettings()
{
    if (! mLoading)
    {
        mSettings->setValue(POWERLOWACTION_KEY, mUi->actionComboBox->itemData(mUi->actionComboBox->currentIndex()).toInt());
        mSettings->setValue(POWERLOWWARNING_KEY, mUi->warningSpinBox->value());
        mSettings->setValue(POWERLOWLEVEL_KEY, mUi->levelSpinBox->value());
    }
}

