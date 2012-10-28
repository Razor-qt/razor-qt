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


#include "razorworldclockconfiguration.h"
#include "ui_razorworldclockconfiguration.h"

#include "razorworldclockconfigurationtimezones.h"


RazorWorldClockConfiguration::RazorWorldClockConfiguration(QSettings &settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RazorWorldClockConfiguration),
    mSettings(settings),
    mOldSettings(settings),
    mLockCascadeSettingChanges(false),
    mConfigurationTimeZones(NULL)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("WorldClockConfigurationWindow");
    ui->setupUi(this);

    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));

    connect(ui->timeZonesLW, SIGNAL(itemSelectionChanged()), SLOT(updateTimeZoneButtons()));
    connect(ui->addPB, SIGNAL(clicked()), SLOT(addTimeZone()));
    connect(ui->removePB, SIGNAL(clicked()), SLOT(removeTimeZone()));
    connect(ui->setAsDefaultPB, SIGNAL(clicked()), SLOT(setTimeZoneAsDefault()));
    connect(ui->moveUpPB, SIGNAL(clicked()), SLOT(moveTimeZoneUp()));
    connect(ui->moveDownPB, SIGNAL(clicked()), SLOT(moveTimeZoneDown()));

    connect(ui->shortFormatRB, SIGNAL(toggled(bool)), SLOT(saveSettings()));
    connect(ui->mediumFormatRB, SIGNAL(toggled(bool)), SLOT(saveSettings()));
    connect(ui->longFormatRB, SIGNAL(toggled(bool)), SLOT(saveSettings()));
    connect(ui->fullFormatRB, SIGNAL(toggled(bool)), SLOT(saveSettings()));
    connect(ui->customFormatRB, SIGNAL(toggled(bool)), SLOT(saveSettings()));
    connect(ui->customFormatPTE, SIGNAL(textChanged()), SLOT(saveSettings()));

    loadSettings();
}

RazorWorldClockConfiguration::~RazorWorldClockConfiguration()
{
    delete ui;
}

void RazorWorldClockConfiguration::loadSettings()
{
    mLockCascadeSettingChanges = true;

    int size = mSettings.beginReadArray("timeZones");
    for (int i = 0; i < size; ++i)
    {
        mSettings.setArrayIndex(i);
        ui->timeZonesLW->addItem(mSettings.value("timeZone", "").toString());
    }
    mSettings.endArray();

    mActiveTimeZone = mSettings.value("activeTimeZone", "").toString();
    if (mActiveTimeZone.isEmpty() && ui->timeZonesLW->count())
        mActiveTimeZone = ui->timeZonesLW->item(0)->text();

    ui->customFormatPTE->setPlainText(mSettings.value("customFormat", "").toString());

    QString formatType = mSettings.value("formatType", "").toString();
    if (formatType == "custom")
        ui->customFormatRB->setChecked(true);
    else if (formatType == "full")
        ui->fullFormatRB->setChecked(true);
    else if (formatType == "long")
        ui->longFormatRB->setChecked(true);
    else if (formatType == "medium")
        ui->mediumFormatRB->setChecked(true);
    else
        ui->shortFormatRB->setChecked(true);

    mLockCascadeSettingChanges = false;
}

void RazorWorldClockConfiguration::saveSettings()
{
    if (mLockCascadeSettingChanges)
        return;

    int size = ui->timeZonesLW->count();
    mSettings.beginWriteArray("timeZones", size);
    for (int i = 0; i < size; ++i)
    {
        mSettings.setArrayIndex(i);
        mSettings.setValue("timeZone", ui->timeZonesLW->item(i)->text());
    }
    mSettings.endArray();

    mSettings.setValue("activeTimeZone", mActiveTimeZone);

    mSettings.setValue("customFormat", ui->customFormatPTE->toPlainText());

    if (ui->customFormatRB->isChecked())
        mSettings.setValue("formatType", "custom");
    else if (ui->fullFormatRB->isChecked())
        mSettings.setValue("formatType", "full");
    else if (ui->longFormatRB->isChecked())
        mSettings.setValue("formatType", "long");
    else if (ui->mediumFormatRB->isChecked())
        mSettings.setValue("formatType", "medium");
    else
        mSettings.setValue("formatType", "short");
}

void RazorWorldClockConfiguration::dialogButtonsAction(QAbstractButton *btn)
{
    if (ui->buttons->buttonRole(btn) == QDialogButtonBox::ResetRole)
    {
        mOldSettings.loadToSettings();
        loadSettings();
    }
    else
        close();
}

void RazorWorldClockConfiguration::updateTimeZoneButtons(void)
{
    int selectedCount = ui->timeZonesLW->selectedItems().count();
    int allCount = ui->timeZonesLW->count();

    ui->removePB->setEnabled(selectedCount != 0);
    ui->setAsDefaultPB->setEnabled(selectedCount == 1);

    bool canMoveUp = false;
    bool canMoveDown = false;
    if ((selectedCount != 0) && (selectedCount != allCount))
    {
        bool skipBottom = true;
        for (int i = allCount - 1; i >= 0; --i)
        {
            if (ui->timeZonesLW->item(i)->isSelected())
            {
                if (!skipBottom)
                {
                    canMoveDown = true;
                    break;
                }
            }
            else
                skipBottom = false;
        }

        bool skipTop = true;
        for (int i = 0; i < allCount; ++i)
        {
            if (ui->timeZonesLW->item(i)->isSelected())
            {
                if (!skipTop)
                {
                    canMoveUp = true;
                    break;
                }
            }
            else
                skipTop = false;
        }
    }
    ui->moveUpPB->setEnabled(canMoveUp);
    ui->moveDownPB->setEnabled(canMoveDown);
}

void RazorWorldClockConfiguration::addTimeZone(void)
{
    if (!mConfigurationTimeZones)
        mConfigurationTimeZones = new RazorWorldClockConfigurationTimeZones(this);

    if (mConfigurationTimeZones->updateAndExec() == QDialog::Accepted)
    {
        if (ui->timeZonesLW->findItems(mConfigurationTimeZones->timeZone(), Qt::MatchExactly).empty())
        {
            QListWidgetItem *item = new QListWidgetItem(mConfigurationTimeZones->timeZone());
            ui->timeZonesLW->addItem(item);
            if (mActiveTimeZone.isEmpty())
                setActive(item);
        }
    }

    saveSettings();
}

void RazorWorldClockConfiguration::removeTimeZone(void)
{
    foreach (QListWidgetItem *item, ui->timeZonesLW->selectedItems())
    {
        if (item->text() == mActiveTimeZone)
            mActiveTimeZone.clear();
        delete item;
    }
    if ((mActiveTimeZone.isEmpty()) && ui->timeZonesLW->count())
        setActive(ui->timeZonesLW->item(0));

    saveSettings();
}

void RazorWorldClockConfiguration::setActive(QListWidgetItem *item)
{
    QFont font = item->font();
    font.setBold(true);
    item->setFont(font);
    mActiveTimeZone = item->text();
}

void RazorWorldClockConfiguration::setTimeZoneAsDefault(void)
{
    QListWidgetItem *item = ui->timeZonesLW->findItems(mActiveTimeZone, Qt::MatchExactly)[0];
    QFont font = item->font();
    font.setBold(false);
    item->setFont(font);

    setActive(ui->timeZonesLW->selectedItems()[0]);

    saveSettings();
}

void RazorWorldClockConfiguration::moveTimeZoneUp(void)
{
    int m = ui->timeZonesLW->count();
    bool skipTop = true;
    for (int i = 0; i < m; ++i)
    {
        if (ui->timeZonesLW->item(i)->isSelected())
        {
            if (!skipTop)
            {
                ui->timeZonesLW->insertItem(i - 1, ui->timeZonesLW->takeItem(i));
                ui->timeZonesLW->item(i - 1)->setSelected(true);
            }
        }
        else
            skipTop = false;
    }

    saveSettings();
}

void RazorWorldClockConfiguration::moveTimeZoneDown(void)
{
    int m = ui->timeZonesLW->count();
    bool skipBottom = true;
    for (int i = m - 1; i >= 0; --i)
    {
        if (ui->timeZonesLW->item(i)->isSelected())
        {
            if (!skipBottom)
            {
                ui->timeZonesLW->insertItem(i + 1, ui->timeZonesLW->takeItem(i));
                ui->timeZonesLW->item(i + 1)->setSelected(true);
            }
        }
        else
            skipBottom = false;
    }

    saveSettings();
}
