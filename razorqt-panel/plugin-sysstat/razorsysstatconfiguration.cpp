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

#include <sysstat/cpustat.hpp>
#include <sysstat/memstat.hpp>
#include <sysstat/netstat.hpp>

#include <QtCore/QSignalMapper>
#include <QtGui/QColorDialog>
#include <QtGui/QFontDialog>


#define CONNECT_SELECT_COLOUR(VAR) \
    connect(ui-> VAR ## B, SIGNAL(clicked()), selectColourMapper, SLOT(map())); \
    selectColourMapper->setMapping(ui-> VAR ## B, QString( #VAR )); \
    showColourMap[QString( #VAR )] = ui-> VAR ## F;

RazorSysStatConfiguration::RazorSysStatConfiguration(QSettings &settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RazorSysStatConfiguration),
    mSettings(settings),
    oldSettings(settings),
    stat(NULL),
    selectColourMapper(new QSignalMapper(this)),
    lockSaving(false)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("SysStatConfigurationWindow");
    ui->setupUi(this);

    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));

    CONNECT_SELECT_COLOUR(bgColour)
    CONNECT_SELECT_COLOUR(gridColour)
    CONNECT_SELECT_COLOUR(titleColour)
    CONNECT_SELECT_COLOUR(cpuSystem)
    CONNECT_SELECT_COLOUR(cpuUser)
    CONNECT_SELECT_COLOUR(cpuNice)
    CONNECT_SELECT_COLOUR(cpuOther)
    CONNECT_SELECT_COLOUR(frequency)
    CONNECT_SELECT_COLOUR(memApps)
    CONNECT_SELECT_COLOUR(memBuffers)
    CONNECT_SELECT_COLOUR(memCached)
    CONNECT_SELECT_COLOUR(swapUsed)
    CONNECT_SELECT_COLOUR(netReceived)
    CONNECT_SELECT_COLOUR(netTransmitted)

    connect(selectColourMapper, SIGNAL(mapped(const QString &)), this, SLOT(selectColour(const QString &)));


    loadSettings();
}

RazorSysStatConfiguration::~RazorSysStatConfiguration()
{
    delete ui;
}

static void applyColor(QFrame *frame, const QColor &color)
{
    QPalette palette = frame->palette();
    palette.setColor(QPalette::Window, color);
    frame->setPalette(palette);
}

void RazorSysStatConfiguration::loadSettings()
{
    lockSaving = true;

    applyColor(ui->bgColourF, QColor(mSettings.value("graph/bgColour", "0x000000").toString().toInt(NULL, 0) | 0xff000000));
    ui->intervalSB->setValue(mSettings.value("graph/updateInterval", 1.0).toDouble());
    ui->sizeSB->setValue(mSettings.value("graph/historyLength", 30).toInt());

    ui->linesSB->setValue(mSettings.value("grid/lines", 1).toInt());
    applyColor(ui->gridColourF, QColor(mSettings.value("grid/colour", "0x808080").toString().toInt(NULL, 0) | 0xff000000));

    ui->labelLE->setText(mSettings.value("title/label", QString()).toString());
    QFont defaultFont(QApplication::font());
    titleFont = QFont(
        mSettings.value("title/font/family", defaultFont.family()).toString(),
        mSettings.value("title/font/pointSize", defaultFont.pointSize()).toInt(),
        mSettings.value("title/font/weight", defaultFont.weight()).toInt(),
        mSettings.value("title/font/italic", defaultFont.italic()).toBool() );
    ui->fontValueL->setText(constructFontDescription(titleFont));
    applyColor(ui->titleColourF, QColor(mSettings.value("title/colour", "0xffffff").toString().toInt(NULL, 0) | 0xff000000));

    int typeIndex = ui->typeCOB->findText(mSettings.value("data/type", QString("CPU")).toString());
    ui->typeCOB->setCurrentIndex((typeIndex >= 0) ? typeIndex : 0);
    on_typeCOB_currentIndexChanged(ui->typeCOB->currentIndex());

    int sourceIndex = ui->sourceCOB->findText(mSettings.value("data/source", QString()).toString());
    ui->sourceCOB->setCurrentIndex((sourceIndex >= 0) ? sourceIndex : 0);

    applyColor(ui->cpuSystemF, QColor(mSettings.value("cpu/systemColour",    "0x800000").toString().toInt(NULL, 0) | 0xff000000));
    applyColor(ui->cpuUserF,   QColor(mSettings.value("cpu/userColour",      "0x000080").toString().toInt(NULL, 0) | 0xff000000));
    applyColor(ui->cpuNiceF,   QColor(mSettings.value("cpu/niceColour",      "0x008000").toString().toInt(NULL, 0) | 0xff000000));
    applyColor(ui->cpuOtherF,  QColor(mSettings.value("cpu/otherColour",     "0x808000").toString().toInt(NULL, 0) | 0xff000000));
    ui->useFrequencyCB->setChecked(mSettings.value("cpu/useFrequency", true).toBool());
    applyColor(ui->frequencyF, QColor(mSettings.value("cpu/frequencyColour", "0x202020").toString().toInt(NULL, 0) | 0xff000000));

    applyColor(ui->memAppsF,    QColor(mSettings.value("mem/appsColour",    "0x000080").toString().toInt(NULL, 0) | 0xff000000));
    applyColor(ui->memBuffersF, QColor(mSettings.value("mem/buffersColour", "0x008000").toString().toInt(NULL, 0) | 0xff000000));
    applyColor(ui->memCachedF,  QColor(mSettings.value("mem/cachedColour",  "0x808000").toString().toInt(NULL, 0) | 0xff000000));
    applyColor(ui->swapUsedF,   QColor(mSettings.value("mem/swapColour",    "0x800000").toString().toInt(NULL, 0) | 0xff000000));

    applyColor(ui->netReceivedF,    QColor(mSettings.value("net/receivedColour",    "0x000080").toString().toInt(NULL, 0) | 0xff000000));
    applyColor(ui->netTransmittedF, QColor(mSettings.value("net/transmittedColour", "0x808000").toString().toInt(NULL, 0) | 0xff000000));
    ui->maximumHS->setValue(PluginSysStat::netSpeedFromString(mSettings.value("net/maximumSpeed", "1 MB/s").toString()));
    on_maximumHS_valueChanged(ui->maximumHS->value());
    ui->logarithmicCB->setChecked(mSettings.value("net/logarithmicScale", true).toBool());
    ui->logScaleSB->setValue(mSettings.value("net/logarithmicScaleSteps", 4).toInt());

    lockSaving = false;
}

void RazorSysStatConfiguration::saveSettings()
{
    if (lockSaving)
        return;

    mSettings.setValue("graph/bgColour", QString("0x%1").arg(ui->bgColourF->palette().color(QPalette::Window).rgb() & 0x00ffffff, 6, 16, QChar('0')));
    mSettings.setValue("graph/updateInterval", ui->intervalSB->value());
    mSettings.setValue("graph/historyLength", ui->sizeSB->value());

    mSettings.setValue("grid/lines", ui->linesSB->value());
    mSettings.setValue("grid/colour", QString("0x%1").arg(ui->gridColourF->palette().color(QPalette::Window).rgb() & 0x00ffffff, 6, 16, QChar('0')));

    mSettings.setValue("title/label", ui->labelLE->text());
    mSettings.setValue("title/font/family", titleFont.family());
    mSettings.setValue("title/font/pointSize", titleFont.pointSize());
    mSettings.setValue("title/font/weight", titleFont.weight());
    mSettings.setValue("title/font/italic", titleFont.italic());
    mSettings.setValue("title/colour", QString("0x%1").arg(ui->titleColourF->palette().color(QPalette::Window).rgb() & 0x00ffffff, 6, 16, QChar('0')));

    mSettings.setValue("data/type", ui->typeCOB->currentText());
    mSettings.setValue("data/source", ui->sourceCOB->currentText());

    mSettings.setValue("cpu/systemColour",    QString("0x%1").arg(ui->cpuSystemF->palette().color(QPalette::Window).rgb() & 0x00ffffff, 6, 16, QChar('0')));
    mSettings.setValue("cpu/userColour",      QString("0x%1").arg(ui->cpuUserF  ->palette().color(QPalette::Window).rgb() & 0x00ffffff, 6, 16, QChar('0')));
    mSettings.setValue("cpu/niceColour",      QString("0x%1").arg(ui->cpuNiceF  ->palette().color(QPalette::Window).rgb() & 0x00ffffff, 6, 16, QChar('0')));
    mSettings.setValue("cpu/otherColour",     QString("0x%1").arg(ui->cpuOtherF ->palette().color(QPalette::Window).rgb() & 0x00ffffff, 6, 16, QChar('0')));
    mSettings.setValue("cpu/useFrequency", ui->useFrequencyCB->isChecked());
    mSettings.setValue("cpu/frequencyColour", QString("0x%1").arg(ui->frequencyF->palette().color(QPalette::Window).rgb() & 0x00ffffff, 6, 16, QChar('0')));

    mSettings.setValue("mem/appsColour",    QString("0x%1").arg(ui->memAppsF   ->palette().color(QPalette::Window).rgb() & 0x00ffffff, 6, 16, QChar('0')));
    mSettings.setValue("mem/buffersColour", QString("0x%1").arg(ui->memBuffersF->palette().color(QPalette::Window).rgb() & 0x00ffffff, 6, 16, QChar('0')));
    mSettings.setValue("mem/cachedColour",  QString("0x%1").arg(ui->memCachedF ->palette().color(QPalette::Window).rgb() & 0x00ffffff, 6, 16, QChar('0')));
    mSettings.setValue("mem/swapColour",    QString("0x%1").arg(ui->swapUsedF  ->palette().color(QPalette::Window).rgb() & 0x00ffffff, 6, 16, QChar('0')));

    mSettings.setValue("net/receivedColour",    QString("0x%1").arg(ui->netReceivedF   ->palette().color(QPalette::Window).rgb() & 0x00ffffff, 6, 16, QChar('0')));
    mSettings.setValue("net/transmittedColour", QString("0x%1").arg(ui->netTransmittedF->palette().color(QPalette::Window).rgb() & 0x00ffffff, 6, 16, QChar('0')));
    mSettings.setValue("net/maximumSpeed", PluginSysStat::netSpeedToString(ui->maximumHS->value()));
    mSettings.setValue("net/logarithmicScale", ui->logarithmicCB->isChecked());
    mSettings.setValue("net/logarithmicScaleSteps", ui->logScaleSB->value());
}

void RazorSysStatConfiguration::dialogButtonsAction(QAbstractButton *btn)
{
    if (ui->buttons->buttonRole(btn) == QDialogButtonBox::ResetRole)
    {
        oldSettings.loadToSettings();
        loadSettings();
    }
    else
        close();
}

void RazorSysStatConfiguration::selectColour(const QString &var)
{
    QFrame *frame = showColourMap[var];
    QPalette palette = frame->palette();
    QColor color = QColorDialog::getColor(palette.color(QPalette::Window), this);
    if (color.isValid())
    {
        palette.setColor(QPalette::Window, color);
        frame->setPalette(palette);

        saveSettings();
    }
}

QString RazorSysStatConfiguration::constructFontDescription(const QFont &font)
{
    QString result(font.family());

    if (font.weight() < QFont::Light)
        result += QString(", ") + tr("Ultra light");
    else if (font.weight() < QFont::Normal)
        result += QString(", ") + tr("Light");
    else if (font.weight() > QFont::Black)
        result += QString(", ") + tr("Ultra black");
    else if (font.weight() > QFont::Bold)
        result += QString(", ") + tr("Black");
    else if (font.weight() > QFont::DemiBold)
        result += QString(", ") + tr("Bold");
    else if (font.weight() > QFont::Normal)
        result += QString(", ") + tr("Demi bold");
//    else
//        result += QString(", ") + tr("Normal");

    if (font.italic())
        result += QString(", ") + tr("Italic");

    result += QString(", %1pt").arg(font.pointSize());
    return result;
}

void RazorSysStatConfiguration::on_fontB_clicked()
{
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, titleFont, this);
    if (ok)
    {
        titleFont = font;
        ui->fontValueL->setText(constructFontDescription(titleFont));

        saveSettings();
    }
}

void RazorSysStatConfiguration::on_typeCOB_currentIndexChanged(int index)
{
    if (stat)
        stat->deleteLater();
    switch (index)
    {
    case 0:
        stat = new SysStat::CpuStat(this);
        break;

    case 1:
        stat = new SysStat::MemStat(this);
        break;

    case 2:
        stat = new SysStat::NetStat(this);
        break;
    }

    ui->sourceCOB->clear();
    ui->sourceCOB->addItems(stat->sources());
    ui->sourceCOB->setCurrentIndex(0);
}

void RazorSysStatConfiguration::on_sourceCOB_currentIndexChanged(int index)
{
    if (ui->typeCOB->currentIndex() == 1) // memory
        ui->memorySW->setCurrentIndex(index);

    saveSettings();
}

void RazorSysStatConfiguration::on_maximumHS_valueChanged(int value)
{
    emit maximumNetSpeedChanged(PluginSysStat::netSpeedToString(value));

    saveSettings();
}
