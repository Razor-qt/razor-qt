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


#include <QtGui/QInputDialog>

#include "razorclockconfiguration.h"
#include "ui_razorclockconfiguration.h"


RazorClockConfiguration::RazorClockConfiguration(QSettings &settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RazorClockConfiguration),
    mSettings(settings),
    oldSettings(settings),
    oldIndex(1)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("ClockConfigurationWindow");
    ui->setupUi(this);

    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), SLOT(dialogButtonsAction(QAbstractButton*)));

    loadSettings();
    /* We use clicked() and activated(int) because these signals aren't emitting after programmaticaly
      change of state */

    connect(ui->dateFormatCOB, SIGNAL(activated(int)), SLOT(dateFormatActivated(int)));

    connect(ui->showSecondsCB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->ampmClockCB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->useUtcCB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->dontShowDateRB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->showDateBeforeTimeRB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->showDateAfterTimeRB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->showDateBelowTimeRB, SIGNAL(clicked()), SLOT(saveSettings()));
}

RazorClockConfiguration::~RazorClockConfiguration()
{
    delete ui;
}

static int currentYear = QDate::currentDate().year();

void RazorClockConfiguration::addDateFormat(const QString &format)
{
    if (ui->dateFormatCOB->findData(QVariant(format)) == -1)
        ui->dateFormatCOB->addItem(QDate(currentYear, 1, 1).toString(format), QVariant(format));
}

void RazorClockConfiguration::createDateFormats()
{
    ui->dateFormatCOB->clear();

    QString systemDateLocale = QLocale::system().dateFormat(QLocale::ShortFormat).toUpper();

    if (systemDateLocale.indexOf("Y") < systemDateLocale.indexOf("D"))
    // Big-endian (year, month, day) -> in some Asia countires like China or Japan
    {
        addDateFormat("MMM d");
        addDateFormat("MMMM d");
        addDateFormat("MMM d, ddd");
        addDateFormat("MMMM d, dddd");
        addDateFormat("yyyy MMM d");
        addDateFormat("yyyy MMMM d");
        addDateFormat("yyyy MMM d, ddd");
        addDateFormat("yyyy MMMM d, dddd");
        addDateFormat("MMM dd");
        addDateFormat("MMMM dd");
        addDateFormat("MMM dd, ddd");
        addDateFormat("MMMM dd, dddd");
        addDateFormat("yyyy MMM dd");
        addDateFormat("yyyy MMMM dd");
        addDateFormat("yyyy MMM dd, ddd");
        addDateFormat("yyyy MMMM dd, dddd");
    }
    else if (systemDateLocale.indexOf("M") < systemDateLocale.indexOf("D"))
    // Middle-endian (month, day, year) -> USA
    {
        addDateFormat("MMM d");
        addDateFormat("MMMM d");
        addDateFormat("ddd, MMM d");
        addDateFormat("dddd, MMMM d");
        addDateFormat("MMM d yyyy");
        addDateFormat("MMMM d yyyy");
        addDateFormat("ddd, MMM d yyyy");
        addDateFormat("dddd, MMMM d yyyy");
        addDateFormat("MMM dd");
        addDateFormat("MMMM dd");
        addDateFormat("ddd, MMM dd");
        addDateFormat("dddd, MMMM dd");
        addDateFormat("MMM dd yyyy");
        addDateFormat("MMMM dd yyyy");
        addDateFormat("ddd, MMM dd yyyy");
        addDateFormat("dddd, MMMM dd yyyy");
    }
    else
    // Little-endian (day, month, year) -> most of Europe
    {
        addDateFormat("d MMM");
        addDateFormat("d MMMM");
        addDateFormat("ddd, d MMM");
        addDateFormat("dddd, d MMMM");
        addDateFormat("d MMM yyyy");
        addDateFormat("d MMMM yyyy");
        addDateFormat("ddd, d MMM yyyy");
        addDateFormat("dddd, d MMMM yyyy");
        addDateFormat("dd MMM");
        addDateFormat("dd MMMM");
        addDateFormat("ddd, dd MMM");
        addDateFormat("dddd, dd MMMM");
        addDateFormat("dd MMM yyyy");
        addDateFormat("dd MMMM yyyy");
        addDateFormat("ddd, dd MMM yyyy");
        addDateFormat("dddd, dd MMMM yyyy");
    }

    addDateFormat(QLocale::system().dateFormat(QLocale::ShortFormat));
    addDateFormat(QLocale::system().dateFormat(QLocale::LongFormat));

    addDateFormat("yyyy-MM-dd"); // ISO

    if (customDateFormat.isEmpty())
        ui->dateFormatCOB->addItem("Custom ...", QVariant(customDateFormat));
    else
        ui->dateFormatCOB->addItem(QString("Custom (%1) ...").arg(QDate(currentYear, 1, 1).toString(customDateFormat)), QVariant(customDateFormat));
}

void RazorClockConfiguration::loadSettings()
{
    QString systemDateLocale = QLocale::system().dateFormat(QLocale::ShortFormat).toUpper();
    QString systemTimeLocale = QLocale::system().timeFormat(QLocale::ShortFormat).toUpper();

    QString timeFormat = mSettings.value("timeFormat", systemTimeLocale.contains("AP") ? "h:mm AP" : "HH:mm").toString();

    ui->showSecondsCB->setChecked(timeFormat.indexOf("ss") > -1);

    ui->ampmClockCB->setChecked(timeFormat.toUpper().indexOf("AP") > -1);

    ui->useUtcCB->setChecked(mSettings.value("UTC", false).toBool());

    ui->dontShowDateRB->setChecked(true);
    ui->showDateBeforeTimeRB->setChecked(mSettings.value("showDate", "no").toString().toLower() == "before");
    ui->showDateAfterTimeRB->setChecked(mSettings.value("showDate", "no").toString().toLower() == "after");
    ui->showDateBelowTimeRB->setChecked(mSettings.value("showDate", "no").toString().toLower() == "below");

    customDateFormat = mSettings.value("customDateFormat", QString()).toString();
    QString dateFormat = mSettings.value("dateFormat", QLocale::system().dateFormat(QLocale::ShortFormat)).toString();

    createDateFormats();

    if (customDateFormat == dateFormat)
        ui->dateFormatCOB->setCurrentIndex(ui->dateFormatCOB->count() - 1);
    else
    {
        ui->dateFormatCOB->setCurrentIndex(ui->dateFormatCOB->findData(dateFormat));
        if (ui->dateFormatCOB->currentIndex() < 0)
            ui->dateFormatCOB->setCurrentIndex(1);
    }
    oldIndex = ui->dateFormatCOB->currentIndex();
}

void RazorClockConfiguration::saveSettings()
{
    QString timeFormat(ui->ampmClockCB->isChecked() ? "h:mm AP" : "HH:mm");

    if (ui->showSecondsCB->isChecked())
        timeFormat.insert(timeFormat.indexOf("mm") + 2, ":ss");

    mSettings.setValue("timeFormat", timeFormat);

    mSettings.setValue("UTC", ui->useUtcCB->isChecked());

    mSettings.setValue("showDate",
        ui->showDateBeforeTimeRB->isChecked() ? "before" :
        (ui->showDateAfterTimeRB->isChecked() ? "after" :
        (ui->showDateBelowTimeRB->isChecked() ? "below" : "no" )));

    mSettings.setValue("customDateFormat", customDateFormat);
    if (ui->dateFormatCOB->currentIndex() == (ui->dateFormatCOB->count() - 1))
        mSettings.setValue("dateFormat", customDateFormat);
    else
        mSettings.setValue("dateFormat", ui->dateFormatCOB->itemData(ui->dateFormatCOB->currentIndex()));
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

void RazorClockConfiguration::dateFormatActivated(int index)
{
    if (index == ui->dateFormatCOB->count() - 1)
    {
        bool ok;
        QString newCustomDateFormat = QInputDialog::getText(this, tr("Input custom date format"), tr(
            "Interpreted sequences of date format are:\n"
            "\n"
            "d\tthe day as number without a leading zero (1 to 31)\n"
            "dd\tthe day as number with a leading zero (01 to 31)\n"
            "ddd\tthe abbreviated localized day name (e.g. 'Mon' to 'Sun').\n"
            "dddd\tthe long localized day name (e.g. 'Monday' to 'Sunday').\n"
            "M\tthe month as number without a leading zero (1-12)\n"
            "MM\tthe month as number with a leading zero (01-12)\n"
            "MMM\tthe abbreviated localized month name (e.g. 'Jan' to 'Dec').\n"
            "MMMM\tthe long localized month name (e.g. 'January' to 'December').\n"
            "yy\tthe year as two digit number (00-99)\n"
            "yyyy\tthe year as four digit number\n"
            "\n"
            "All other input characters will be treated as text.\n"
            "Any sequence of characters that are enclosed in single quotes (')\n"
            "will also be treated as text and not be used as an expression.\n"
            "\n"
            "\n"
            "Custom date format:"
            ), QLineEdit::Normal, customDateFormat, &ok);
        if (ok)
        {
            customDateFormat = newCustomDateFormat;
            oldIndex = index;
            createDateFormats();
        }
        ui->dateFormatCOB->setCurrentIndex(oldIndex);
    }
    else
        oldIndex = index;

    saveSettings();
}
