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

#include <QtGui/QFileDialog>

#include "advancedsettings.h"


AdvancedSettings::AdvancedSettings(RazorSettings* settings, QWidget *parent):
    QWidget(parent),
    mSettings(settings)
{
    setupUi(this);
    restoreSettings();

    connect(serverDecidesBox, SIGNAL(valueChanged(int)), this, SLOT(save()));
    connect(spacingBox, SIGNAL(valueChanged(int)), this, SLOT(save()));
    connect(widthBox, SIGNAL(valueChanged(int)), this, SLOT(save()));
}

AdvancedSettings::~AdvancedSettings()
{
}

void AdvancedSettings::restoreSettings()
{
    int serverDecides = mSettings->value("server_decides", 10).toInt();
    if (serverDecides <= 0)
        serverDecides = 10;
    serverDecidesBox->setValue(serverDecides);

    spacingBox->setValue(mSettings->value("spacing", 6).toInt());
    widthBox->setValue(mSettings->value("width", 300).toInt());
}

void AdvancedSettings::save()
{
    mSettings->setValue("server_decides", serverDecidesBox->value());
    mSettings->setValue("spacing", spacingBox->value());
    mSettings->setValue("width", widthBox->value());

    mSettings->sync();
}

