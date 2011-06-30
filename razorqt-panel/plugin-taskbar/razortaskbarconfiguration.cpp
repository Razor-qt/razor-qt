/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Maciej Płaza <plaza.maciej@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "razortaskbarconfiguration.h"
#include "ui_razortaskbarconfiguration.h"

RazorTaskbarConfiguration::RazorTaskbarConfiguration(QSettings &settings, QWidget *parent):
    QDialog(parent),
    ui(new Ui::RazorTaskbarConfiguration),
    mSettings(settings)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("TaskbarConfigurationWindow");
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(rejectChanges()));

    ui->buttonStyleCB->addItem("Icon and text", "IconText");
    ui->buttonStyleCB->addItem("Only icon", "Icon");
    ui->buttonStyleCB->addItem("Only text", "Text");

    loadSettings();
    connect(ui->fAllDesktopsCB, SIGNAL(toggled(bool)), this, SLOT(saveSettings()));
    connect(ui->fCurrentDesktopRB, SIGNAL(toggled(bool)), this, SLOT(saveSettings()));
    connect(ui->buttonStyleCB, SIGNAL(currentIndexChanged(int)), this, SLOT(updateControls(int)));
    connect(ui->buttonStyleCB, SIGNAL(currentIndexChanged(int)), this, SLOT(saveSettings()));
    connect(ui->maxWidthSB, SIGNAL(valueChanged(int)), this, SLOT(saveSettings()));
}

RazorTaskbarConfiguration::~RazorTaskbarConfiguration()
{
    delete ui;
}

void RazorTaskbarConfiguration::loadSettings()
{
    QVariant buffer;

    buffer = mSettings.value("showOnlyCurrentDesktopTasks", false);
    oldSettings.insert("showOnlyCurrentDesktopTasks", buffer);
    if (buffer.toBool() == true)
    {
        ui->fCurrentDesktopRB->setChecked(true);
    }
    else
    {
        ui->fAllDesktopsCB->setChecked(true);
    }

    buffer = mSettings.value("buttonStyle", "IconText");
    ui->buttonStyleCB->setCurrentIndex(ui->buttonStyleCB->findData(buffer));
    updateControls(ui->buttonStyleCB->currentIndex());
    oldSettings.insert("buttonStyle", buffer);

    buffer = mSettings.value("maxWidth", 400);
    ui->maxWidthSB->setValue(buffer.toInt());
    oldSettings.insert("maxWidth", buffer);
}

void RazorTaskbarConfiguration::saveSettings()
{
    mSettings.setValue("showOnlyCurrentDesktopTasks", ui->fCurrentDesktopRB->isChecked());
    mSettings.setValue("buttonStyle", ui->buttonStyleCB->itemData(ui->buttonStyleCB->currentIndex()));
    mSettings.setValue("maxWidth", ui->maxWidthSB->value());
}

void RazorTaskbarConfiguration::rejectChanges()
{
    QHash<QString, QVariant>::const_iterator i = oldSettings.constBegin();

    while(i != oldSettings.constEnd())
    {
        mSettings.setValue(i.key(), i.value());
        ++i;
    }
}

void RazorTaskbarConfiguration::updateControls(int index)
{
    if (ui->buttonStyleCB->itemData(index) == "Icon")
    {
        ui->maxWidthSB->setEnabled(false);
        ui->maxWidthL->setEnabled(false);
    }
    else
    {
        ui->maxWidthSB->setEnabled(true);
        ui->maxWidthL->setEnabled(true);
    }
}
