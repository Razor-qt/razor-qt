/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Maciej Płaza <plaza.maciej@gmail.com>
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


#include "razorclockconfiguration.h"
#include "ui_razorclockconfiguration.h"

#include <QtGui/QFontDialog>

RazorClockConfiguration::RazorClockConfiguration(QSettings &settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RazorClockConfiguration),
    mSettings(settings),
    oldSettings(settings)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("ClockConfigurationWindow");
    ui->setupUi(this);

    createDateFormats();

    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));

    loadSettings();
    /* We use clicked() and activated(int) because these signals aren't emitting after programmaticaly
      change of state */

    connect(ui->showDateCB, SIGNAL(toggled(bool)), this, SLOT(enableDateFont()));
    connect(ui->dateOnNewLineCB, SIGNAL(toggled(bool)), this, SLOT(enableDateFont()));
    connect(ui->useThemeFontCB, SIGNAL(toggled(bool)), this, SLOT(enableDateFont()));

    connect(ui->showSecondsCB, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->ampmClockCB, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->showDateCB, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->dateOnNewLineCB, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->dateFormatCOB, SIGNAL(activated(int)), this, SLOT(saveSettings()));
    connect(ui->timeFontB, SIGNAL(clicked()), this, SLOT(changeTimeFont()));
    connect(ui->dateFontB, SIGNAL(clicked()), this, SLOT(changeDateFont()));
    updateEnableDateFont();
}

RazorClockConfiguration::~RazorClockConfiguration()
{
    delete ui;
}

void RazorClockConfiguration::createDateFormats()
{
    QString systemLocale = QLocale::system().dateFormat(QLocale::ShortFormat);
    ui->dateFormatCOB->addItem(QDate::currentDate().toString(systemLocale), QVariant(systemLocale));
    systemLocale = systemLocale.toUpper();

    if (systemLocale.indexOf("Y") < systemLocale.indexOf("D"))
    // Big-endian (year, month, day) -> in some Asia countires like China or Japan
    {
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("MMM dd"), QVariant("MMM dd"));
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("MMMM dd"), QVariant("MMMM dd"));
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("yyyy MMM dd"), QVariant("yyyy MMM dd"));
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("ddd, MMM dd"), QVariant("ddd, MMM dd"));
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("ddd, MMMM dd"), QVariant("ddd, MMMM dd"));
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("ddd, yyyy MMM dd"), QVariant("ddd, yyyy MMM dd"));
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("dddd, yyyy MMMM dd"), QVariant("dddd, yyyy MMMM dd"));
    }
    else if (systemLocale.indexOf("M") < systemLocale.indexOf("D"))
    // Middle-endian (month, day, year) -> USA
    {
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("MMM dd"), QVariant("MMM dd"));
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("MMMM dd"), QVariant("MMMM dd"));
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("MMM dd yyyy"), QVariant("MMM dd yyyy"));
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("ddd, MMM dd"), QVariant("ddd, MMM dd"));
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("ddd, MMMM dd"), QVariant("ddd, MMMM dd"));
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("ddd, MMM dd yyyy"), QVariant("ddd, MMM dd yyyy"));
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("dddd, MMMM dd yyyy"), QVariant("dddd, MMMM dd yyyy"));
    }
    else
    // Little-endian (day, month, year) -> most of Europe
    {
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("dd MMM"), QVariant("dd MMM"));
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("dd MMMM"), QVariant("dd MMMM"));
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("dd MMM yyyy"), QVariant("dd MMM yyyy"));
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("ddd, dd MMM"), QVariant("ddd, dd MMM"));
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("ddd, dd MMMM"), QVariant("ddd, dd MMMM"));
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("ddd, dd MMM yyyy"), QVariant("ddd, dd MMM yyyy"));
        ui->dateFormatCOB->addItem(QDate::currentDate().toString("dddd, dd MMMM yyyy"), QVariant("dddd, dd MMMM yyyy"));
    }
}

void RazorClockConfiguration::loadSettings()
{
    QString timeFormat;

    ui->showDateCB->setChecked(mSettings.value("showDate", false).toBool());
    ui->dateOnNewLineCB->setChecked(mSettings.value("dateOnNewLine", true).toBool());

    ui->dateFormatCOB->setCurrentIndex(ui->dateFormatCOB->findData(mSettings.value("dateFormat", Qt::SystemLocaleShortDate)));
    if (ui->dateFormatCOB->currentIndex() < 0)
    {
        ui->dateFormatCOB->setCurrentIndex(1);
    }

    if (QLocale::system().timeFormat(QLocale::ShortFormat).toUpper().contains("AP") == true)
    {
        timeFormat = mSettings.value("timeFormat", "h:mm AP").toString();
    }
    else
    {
        timeFormat = mSettings.value("timeFormat", "HH:mm").toString();
    }

    if (timeFormat.indexOf("ss") > -1)
    {
        ui->showSecondsCB->setChecked(true);
    }
    else
    {
        ui->showSecondsCB->setChecked(false);
    }

    if (timeFormat.toUpper().indexOf("AP") > -1)
    {
        ui->ampmClockCB->setChecked(true);
    }
    else
    {
        ui->ampmClockCB->setChecked(false);
    }

    QFont defaultFont(QApplication::font());

    timeFont = QFont(
        mSettings.value("timeFont/family", defaultFont.family()).toString(),
        mSettings.value("timeFont/pointSize", defaultFont.pointSize()).toInt(),
        mSettings.value("timeFont/weight", defaultFont.weight()).toInt(),
        mSettings.value("timeFont/italic", defaultFont.italic()).toBool() );

    dateFont = QFont(
        mSettings.value("dateFont/family", defaultFont.family()).toString(),
        mSettings.value("dateFont/pointSize", defaultFont.pointSize()).toInt(),
        mSettings.value("dateFont/weight", defaultFont.weight()).toInt(),
        mSettings.value("dateFont/italic", defaultFont.italic()).toBool() );

    ui->useThemeFontCB->setChecked(mSettings.value("useThemeFonts", true).toBool());

    ui->timeFontB->setText(constructFontDescription(timeFont));
    ui->dateFontB->setText(constructFontDescription(dateFont));
}

void RazorClockConfiguration::saveSettings()
{
    QString timeFormat;

    mSettings.setValue("showDate", ui->showDateCB->isChecked());
    mSettings.setValue("dateOnNewLine", ui->dateOnNewLineCB->isChecked());
    mSettings.setValue("dateFormat", ui->dateFormatCOB->itemData(ui->dateFormatCOB->currentIndex()));

    if (ui->ampmClockCB->isChecked() == true)
    {
        timeFormat = "h:mm AP";
    }
    else
    {
        timeFormat = "HH:mm";
    }

    if (ui->showSecondsCB->isChecked() == true)
    {
        timeFormat.insert(timeFormat.indexOf("mm") + 2, ":ss");
    }

    mSettings.setValue("timeFormat", timeFormat);

    mSettings.setValue("timeFont/family", timeFont.family());
    mSettings.setValue("timeFont/pointSize", timeFont.pointSize());
    mSettings.setValue("timeFont/weight", timeFont.weight());
    mSettings.setValue("timeFont/italic", timeFont.italic());

    mSettings.setValue("dateFont/family", dateFont.family());
    mSettings.setValue("dateFont/pointSize", dateFont.pointSize());
    mSettings.setValue("dateFont/weight", dateFont.weight());
    mSettings.setValue("dateFont/italic", dateFont.italic());

    mSettings.setValue("useThemeFonts", ui->useThemeFontCB->isChecked());
}

void RazorClockConfiguration::changeTimeFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, timeFont, this, tr("Time font") );
    if (ok)
    {
        timeFont = font;
        ui->timeFontB->setText(constructFontDescription(timeFont));
        saveSettings();
    }
}

void RazorClockConfiguration::changeDateFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, dateFont, this, tr("Date font") );
    if (ok)
    {
        dateFont = font;
        ui->dateFontB->setText(constructFontDescription(dateFont));
        saveSettings();
    }
}

QString RazorClockConfiguration::constructFontDescription(const QFont &font)
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

void RazorClockConfiguration::dialogButtonsAction(QAbstractButton *btn)
{
    if (ui->buttons->buttonRole(btn) == QDialogButtonBox::ResetRole)
    {
        oldSettings.loadToSettings();
        loadSettings();
    }
    else
    {
        close();
    }
}

void RazorClockConfiguration::updateEnableDateFont()
{
    bool enable = ui->showDateCB->isChecked() && ui->dateOnNewLineCB->isChecked() && !ui->useThemeFontCB->isChecked();
    ui->dateFontB->setEnabled(enable);
    ui->dateFontL->setEnabled(enable);
}

void RazorClockConfiguration::enableDateFont()
{
    updateEnableDateFont();

    saveSettings();
}
