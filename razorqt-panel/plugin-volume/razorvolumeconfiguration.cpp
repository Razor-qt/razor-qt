/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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

#include "razorvolumeconfiguration.h"
#include "ui_razorvolumeconfiguration.h"

#include "pulseaudiodevice.h"
#include "pulseaudioengine.h"

#include <QComboBox>
#include <QDebug>

RazorVolumeConfiguration::RazorVolumeConfiguration(QSettings &settings, QWidget *parent) :
    RazorPanelPluginConfigDialog(settings, parent),
    ui(new Ui::RazorVolumeConfiguration)
{
    ui->setupUi(this);

    loadSettings();
    connect(ui->devAddedCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(sinkSelectionChanged(int)));
    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));
}

RazorVolumeConfiguration::~RazorVolumeConfiguration()
{
    delete ui;
}

void RazorVolumeConfiguration::setSinkList(const QList<PulseAudioDevice *> sinks)
{
    ui->devAddedCombo->clear();

    foreach (const PulseAudioDevice *dev, sinks) {
        ui->devAddedCombo->addItem(dev->description, dev->index);
    }
}

void RazorVolumeConfiguration::sinkSelectionChanged(int index)
{
    settings().setValue("defaultSink", index);
}

void RazorVolumeConfiguration::loadSettings()
{
    setComboboxIndexByData(ui->devAddedCombo, settings().value("defaultSink", 0), 1);
}

