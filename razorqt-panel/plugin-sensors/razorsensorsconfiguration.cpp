/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Łukasz Twarduś <ltwardus@gmail.com>
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
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#include "razorsensorsconfiguration.h"
#include "ui_razorsensorsconfiguration.h"
#include <QtCore/QStringList>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include <QtGui/QColorDialog>
#include <QtCore/QDebug>

RazorSensorsConfiguration::RazorSensorsConfiguration(QSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RazorSensorsConfiguration),
    mSettings(settings),
    oldSettings(settings)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("SensorsConfigurationWindow");
    ui->setupUi(this);

    // We load settings here cause we have to set up dynamic widgets
    loadSettings();

    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));
    connect(ui->updateIntervalSB, SIGNAL(valueChanged(int)), this, SLOT(saveSettings()));
    connect(ui->tempBarWidthSB, SIGNAL(valueChanged(int)), this, SLOT(saveSettings()));
    connect(ui->detectedChipsCB, SIGNAL(activated(int)), this, SLOT(detectedChipSelected(int)));
    connect(ui->celsiusTempScaleRB, SIGNAL(toggled(bool)), this, SLOT(saveSettings()));
    // We don't need signal from the other radio box as celsiusTempScaleRB will send one
    //connect(ui->fahrenheitTempScaleRB, SIGNAL(toggled(bool)), this, SLOT(saveSettings()));
    connect(ui->warningAboutHighTemperatureChB, SIGNAL(toggled(bool)), this, SLOT(saveSettings()));

    /**
     * Signals for enable/disable and bar color change are set in the loadSettings method because
     * we are creating them dynamically.
     */
}

RazorSensorsConfiguration::~RazorSensorsConfiguration()
{
    delete ui;
}

void RazorSensorsConfiguration::loadSettings()
{
    ui->updateIntervalSB->setValue(mSettings->value("updateInterval").toInt());
    ui->tempBarWidthSB->setValue(mSettings->value("tempBarWidth").toInt());

    if (mSettings->value("useFahrenheitScale").toBool())
    {
        ui->fahrenheitTempScaleRB->setChecked(true);
    }

    // In case of reloading settings we have to clear GUI elements
    ui->detectedChipsCB->clear();

    mSettings->beginGroup("chips");
    QStringList chipNames = mSettings->childGroups();

    for (int i = 0; i < chipNames.size(); ++i)
    {
        ui->detectedChipsCB->addItem(chipNames[i]);
    }
    mSettings->endGroup();

    // Load feature for the first chip if exist
    if (chipNames.size() > 0)
    {
        detectedChipSelected(0);
    }

    ui->warningAboutHighTemperatureChB->setChecked(
            mSettings->value("warningAboutHighTemperature").toBool());
}

void RazorSensorsConfiguration::saveSettings()
{
    mSettings->setValue("updateInterval", ui->updateIntervalSB->value());
    mSettings->setValue("tempBarWidth", ui->tempBarWidthSB->value());

    if (ui->fahrenheitTempScaleRB->isChecked())
    {
        mSettings->setValue("useFahrenheitScale", true);
    }
    else
    {
        mSettings->setValue("useFahrenheitScale", false);
    }

    mSettings->beginGroup("chips");
    QStringList chipNames = mSettings->childGroups();

    if (chipNames.size())
    {
        QStringList chipFeatureLabels;
        QPushButton* colorButton = NULL;
        QCheckBox* enabledCheckbox = NULL;

        mSettings->beginGroup(chipNames[ui->detectedChipsCB->currentIndex()]);

        chipFeatureLabels = mSettings->childGroups();
        for (int j = 0; j < chipFeatureLabels.size(); ++j)
        {
            mSettings->beginGroup(chipFeatureLabels[j]);

            enabledCheckbox = qobject_cast<QCheckBox*>(ui->chipFeaturesT->cellWidget(j, 0));
            // We know what we are doing so we don't have to check if enabledCheckbox == 0
            mSettings->setValue("enabled", enabledCheckbox->isChecked());

            colorButton = qobject_cast<QPushButton*>(ui->chipFeaturesT->cellWidget(j, 2));
            // We know what we are doing so we don't have to check if colorButton == 0
            mSettings->setValue(
                    "color",
                    colorButton->palette().color(QPalette::Normal, QPalette::Button).name());

            mSettings->endGroup();
        }
        mSettings->endGroup();

    }
    mSettings->endGroup();

    mSettings->setValue("warningAboutHighTemperature",
                       ui->warningAboutHighTemperatureChB->isChecked());
}

void RazorSensorsConfiguration::dialogButtonsAction(QAbstractButton *btn)
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

void RazorSensorsConfiguration::changeProgressBarColor()
{
    QAbstractButton* btn = qobject_cast<QAbstractButton*>(sender());

    if (btn)
    {
        QPalette pal = btn->palette();
        QColor color = QColorDialog::getColor(pal.color(QPalette::Normal, QPalette::Button), this);

        if (color.isValid())
        {
            pal.setColor(QPalette::Normal, QPalette::Button, color);
            btn->setPalette(pal);
            saveSettings();
        }
    }
    else
    {
        qDebug() << "RazorSensorsConfiguration::changeProgressBarColor():" << "invalid button cast";
    }
}

void RazorSensorsConfiguration::detectedChipSelected(int index)
{
    mSettings->beginGroup("chips");
    QStringList chipNames = mSettings->childGroups();
    QStringList chipFeatureLabels;
    QPushButton* colorButton = NULL;
    QCheckBox* enabledCheckbox = NULL;
    QTableWidgetItem *chipFeatureLabel = NULL;

    if (index < chipNames.size())
    {
        qDebug() << "Selected chip: " << ui->detectedChipsCB->currentText();

        // In case of reloading settings we have to clear GUI elements
        ui->chipFeaturesT->setRowCount(0);

        // Add detected chips and features
        QStringList chipFeaturesLabels;
        chipFeaturesLabels << tr("Enabled") << tr("Label") << tr("Color");
        ui->chipFeaturesT->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
        ui->chipFeaturesT->setHorizontalHeaderLabels(chipFeaturesLabels);

        mSettings->beginGroup(chipNames[index]);
        chipFeatureLabels = mSettings->childGroups();
        for (int j = 0; j < chipFeatureLabels.size(); ++j)
        {
            mSettings->beginGroup(chipFeatureLabels[j]);

            ui->chipFeaturesT->insertRow(j);

            enabledCheckbox = new QCheckBox(ui->chipFeaturesT);
            enabledCheckbox->setChecked(mSettings->value("enabled").toBool());
            // Connect here after the setChecked call because we don't want to send signal
            connect(enabledCheckbox, SIGNAL(stateChanged(int)), this, SLOT(saveSettings()));
            ui->chipFeaturesT->setCellWidget(j, 0, enabledCheckbox);

            chipFeatureLabel = new QTableWidgetItem(chipFeatureLabels[j]);
            chipFeatureLabel->setFlags(Qt::ItemIsEnabled);
            ui->chipFeaturesT->setItem(j, 1, chipFeatureLabel);

            colorButton = new QPushButton(ui->chipFeaturesT);
            connect(colorButton, SIGNAL(clicked()), this, SLOT(changeProgressBarColor()));
            QPalette pal = colorButton->palette();
            pal.setColor(QPalette::Normal, QPalette::Button,
                         QColor(mSettings->value("color").toString()));
            colorButton->setPalette(pal);
            ui->chipFeaturesT->setCellWidget(j, 2, colorButton);

            mSettings->endGroup();
        }
        mSettings->endGroup();
    }
    else
    {
        qDebug() << "Invalid chip index: " << index;
    }

    mSettings->endGroup();
}

