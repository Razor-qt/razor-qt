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


RazorWorldClockConfiguration::RazorWorldClockConfiguration(QSettings &settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RazorWorldClockConfiguration),
    mSettings(settings),
    oldSettings(settings)
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
//    ui->...->setValue( mSettings.value("...", 0).toInt() );
}

void RazorWorldClockConfiguration::saveSettings()
{
//    mSettings.setValue("...", ui->...->...());
}

void RazorWorldClockConfiguration::dialogButtonsAction(QAbstractButton *btn)
{
    if (ui->buttons->buttonRole(btn) == QDialogButtonBox::ResetRole)
    {
        oldSettings.loadToSettings();
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
//    ui->timeZonesLW->addItem(...);
}

void RazorWorldClockConfiguration::removeTimeZone(void)
{
    foreach (QListWidgetItem *item, ui->timeZonesLW->selectedItems())
        delete item;
}

void RazorWorldClockConfiguration::setTimeZoneAsDefault(void)
{

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
}
