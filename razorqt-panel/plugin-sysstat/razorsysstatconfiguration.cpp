/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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


#include "razorsysstatconfiguration.h"
#include "ui_razorsysstatconfiguration.h"
#include "razorsysstatutils.h"
#include "razorsysstatcolours.h"

#include <sysstat/cpustat.h>
#include <sysstat/memstat.h>
#include <sysstat/netstat.h>


RazorSysStatConfiguration::RazorSysStatConfiguration(QSettings &settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RazorSysStatConfiguration),
    mSettings(settings),
    oldSettings(settings),
    mStat(NULL),
    mLockSaving(false),
    mColoursDialog(NULL)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("SysStatConfigurationWindow");
    ui->setupUi(this);


    loadSettings();
}

RazorSysStatConfiguration::~RazorSysStatConfiguration()
{
    delete ui;
}

void RazorSysStatConfiguration::loadSettings()
{
    mLockSaving = true;

    ui->intervalSB->setValue(mSettings.value("graph/updateInterval", 1.0).toDouble());
    ui->sizeSB->setValue(mSettings.value("graph/minimalSize", 30).toInt());

    ui->linesSB->setValue(mSettings.value("grid/lines", 1).toInt());

    ui->titleLE->setText(mSettings.value("title/label", QString()).toString());

    int typeIndex = ui->typeCOB->findText(mSettings.value("data/type", QString("CPU")).toString());
    ui->typeCOB->setCurrentIndex((typeIndex >= 0) ? typeIndex : 0);
    on_typeCOB_currentIndexChanged(ui->typeCOB->currentIndex());

    int sourceIndex = ui->sourceCOB->findText(mSettings.value("data/source", QString()).toString());
    ui->sourceCOB->setCurrentIndex((sourceIndex >= 0) ? sourceIndex : 0);

    ui->useFrequencyCB->setChecked(mSettings.value("cpu/useFrequency", true).toBool());
    ui->maximumHS->setValue(PluginSysStat::netSpeedFromString(mSettings.value("net/maximumSpeed", "1 MB/s").toString()));
    on_maximumHS_valueChanged(ui->maximumHS->value());
    ui->logarithmicCB->setChecked(mSettings.value("net/logarithmicScale", true).toBool());
    ui->logScaleSB->setValue(mSettings.value("net/logarithmicScaleSteps", 4).toInt());

    bool useThemeColours = mSettings.value("graph/useThemeColours", true).toBool();
    ui->useThemeColoursRB->setChecked(useThemeColours);
    ui->useCustomColoursRB->setChecked(!useThemeColours);
    ui->customColoursB->setEnabled(!useThemeColours);

    mLockSaving = false;
}

void RazorSysStatConfiguration::saveSettings()
{
    if (mLockSaving)
        return;

    mSettings.setValue("graph/useThemeColours", ui->useThemeColoursRB->isChecked());
    mSettings.setValue("graph/updateInterval", ui->intervalSB->value());
    mSettings.setValue("graph/minimalSize", ui->sizeSB->value());

    mSettings.setValue("grid/lines", ui->linesSB->value());

    mSettings.setValue("title/label", ui->titleLE->text());

    mSettings.setValue("data/type", ui->typeCOB->currentText());
    mSettings.setValue("data/source", ui->sourceCOB->currentText());

    mSettings.setValue("cpu/useFrequency", ui->useFrequencyCB->isChecked());

    mSettings.setValue("net/maximumSpeed", PluginSysStat::netSpeedToString(ui->maximumHS->value()));
    mSettings.setValue("net/logarithmicScale", ui->logarithmicCB->isChecked());
    mSettings.setValue("net/logarithmicScaleSteps", ui->logScaleSB->value());
}

void RazorSysStatConfiguration::on_buttons_clicked(QAbstractButton *btn)
{
    if (ui->buttons->buttonRole(btn) == QDialogButtonBox::ResetRole)
    {
        oldSettings.loadToSettings();
        loadSettings();
    }
    else
        close();
}

void RazorSysStatConfiguration::on_typeCOB_currentIndexChanged(int index)
{
    if (mStat)
        mStat->deleteLater();
    switch (index)
    {
    case 0:
        mStat = new SysStat::CpuStat(this);
        break;

    case 1:
        mStat = new SysStat::MemStat(this);
        break;

    case 2:
        mStat = new SysStat::NetStat(this);
        break;
    }

    ui->sourceCOB->clear();
    ui->sourceCOB->addItems(mStat->sources());
    ui->sourceCOB->setCurrentIndex(0);
}

void RazorSysStatConfiguration::on_maximumHS_valueChanged(int value)
{
    emit maximumNetSpeedChanged(PluginSysStat::netSpeedToString(value));

    saveSettings();
}

void RazorSysStatConfiguration::coloursChanged()
{
    const RazorSysStatColours::Colours &colours = mColoursDialog->colours();

    mSettings.setValue("grid/colour",  colours["grid"].name());
    mSettings.setValue("title/colour", colours["title"].name());

    mSettings.setValue("cpu/systemColour",    colours["cpuSystem"].name());
    mSettings.setValue("cpu/userColour",      colours["cpuUser"].name());
    mSettings.setValue("cpu/niceColour",      colours["cpuNice"].name());
    mSettings.setValue("cpu/otherColour",     colours["cpuOther"].name());
    mSettings.setValue("cpu/frequencyColour", colours["cpuFrequency"].name());

    mSettings.setValue("mem/appsColour",    colours["memApps"].name());
    mSettings.setValue("mem/buffersColour", colours["memBuffers"].name());
    mSettings.setValue("mem/cachedColour",  colours["memCached"].name());
    mSettings.setValue("mem/swapColour",    colours["memSwap"].name());

    mSettings.setValue("net/receivedColour",    colours["netReceived"].name());
    mSettings.setValue("net/transmittedColour", colours["netTransmitted"].name());
}

void RazorSysStatConfiguration::on_customColoursB_clicked()
{
    if (!mColoursDialog)
    {
        mColoursDialog = new RazorSysStatColours(this);
        connect(mColoursDialog, SIGNAL(coloursChanged()), SLOT(coloursChanged()));
    }

    RazorSysStatColours::Colours colours;

    const RazorSysStatColours::Colours &defaultColours = mColoursDialog->defaultColours();

    colours["grid"]  = QColor(mSettings.value("grid/colour",  defaultColours["grid"] .name()).toString());
    colours["title"] = QColor(mSettings.value("title/colour", defaultColours["title"].name()).toString());

    colours["cpuSystem"]    = QColor(mSettings.value("cpu/systemColour",    defaultColours["cpuSystem"]   .name()).toString());
    colours["cpuUser"]      = QColor(mSettings.value("cpu/userColour",      defaultColours["cpuUser"]     .name()).toString());
    colours["cpuNice"]      = QColor(mSettings.value("cpu/niceColour",      defaultColours["cpuNice"]     .name()).toString());
    colours["cpuOther"]     = QColor(mSettings.value("cpu/otherColour",     defaultColours["cpuOther"]    .name()).toString());
    colours["cpuFrequency"] = QColor(mSettings.value("cpu/frequencyColour", defaultColours["cpuFrequency"].name()).toString());

    colours["memApps"]    = QColor(mSettings.value("mem/appsColour",    defaultColours["memApps"]   .name()).toString());
    colours["memBuffers"] = QColor(mSettings.value("mem/buffersColour", defaultColours["memBuffers"].name()).toString());
    colours["memCached"]  = QColor(mSettings.value("mem/cachedColour",  defaultColours["memCached"] .name()).toString());
    colours["memSwap"]    = QColor(mSettings.value("mem/swapColour",    defaultColours["memSwap"]   .name()).toString());

    colours["netReceived"]    = QColor(mSettings.value("net/receivedColour",    defaultColours["netReceived"]   .name()).toString());
    colours["netTransmitted"] = QColor(mSettings.value("net/transmittedColour", defaultColours["netTransmitted"].name()).toString());

    mColoursDialog->setColours(colours);

    mColoursDialog->exec();
}
