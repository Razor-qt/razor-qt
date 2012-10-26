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
    mUi(new Ui::LidSettings),
    mLoading(false)
{
    mSettings = settings;
    mUi->setupUi(this);
    connect(mUi->actionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(saveAction()));
}

LidSettings::~LidSettings()
{
    delete mUi;
}


void LidSettings::loadSettings()
{
    mLoading = true;
    mUi->actionComboBox->clear();
    mUi->actionComboBox->addItem(tr("Nothing"), NOTHING);
    mUi->actionComboBox->addItem(tr("Sleep"), SLEEP);
    mUi->actionComboBox->addItem(tr("Hibernate"), HIBERNATE);
    mUi->actionComboBox->addItem(tr("Shutdown"), POWEROFF);

    for (int index = 0; index < mUi->actionComboBox->count(); index++)
    {
        if (mSettings->value(LIDCLOSEDACTION_KEY, 0) == mUi->actionComboBox->itemData(index).toInt())
        {
            mUi->actionComboBox->setCurrentIndex(index);
            break;
        }
    }
    mLoading = false;
}

void LidSettings::saveAction()
{
    if (! mLoading)
    {
        mSettings->setValue(LIDCLOSEDACTION_KEY, mUi->actionComboBox->itemData(mUi->actionComboBox->currentIndex()).toInt());
    }
}
