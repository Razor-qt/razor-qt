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

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "razorsensors.h"
#include "razorsensorsconfiguration.h"
#include <QtGui/QMessageBox>
#include <QtGui/QPalette>
#include <QtCore/QDebug>

EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorSensors)

RazorSensors::RazorSensors(const RazorPanelPluginStartInfo* startInfo, QWidget* parent):
        RazorPanelPlugin(startInfo, parent),
        mWarningAboutHighTemperatureTimerFreq(500)
{
    setObjectName("Sensors");
    connect(panel(), SIGNAL(panelRealigned()), this, SLOT(realign()));

    mDetectedChips = mSensors.getDetectedChips();

    /**
     * We have all needed data to initialize default settings, we have to do it here as later
     * we are using them.
     */
    initDefaultSettings();

    // Add GUI elements
    QProgressBar* pg = NULL;

    QString chipFeatureLabel;

    settings().beginGroup("chips");

    for (unsigned int i = 0; i < mDetectedChips.size(); ++i)
    {
        settings().beginGroup(QString::fromStdString(mDetectedChips[i].getName()));
        const std::vector<Feature>& features = mDetectedChips[i].getFeatures();

        for (unsigned int j = 0; j < features.size(); ++j)
        {
            if (features[j].getType() == SENSORS_FEATURE_TEMP)
            {
                chipFeatureLabel = QString::fromStdString(features[j].getLabel());
                settings().beginGroup(chipFeatureLabel);

                pg = new QProgressBar(this);

                // Hide progress bar if it is not enabled
                if (!settings().value("enabled").toBool())
                {
                    pg->hide();
                }

                pg->setToolTip(chipFeatureLabel);
                pg->setTextVisible(false);

                QPalette pal = pg->palette();
                QColor color(settings().value("color").toString());
                pal.setColor(QPalette::Active, QPalette::Highlight, color);
                pal.setColor(QPalette::Inactive, QPalette::Highlight, color);
                pg->setPalette(pal);

                mTemperatureProgressBars.push_back(pg);
                layout()->addWidget(pg);

                settings().endGroup();
            }
        }
        settings().endGroup();
    }

    settings().endGroup();

    // Fit plugin to current panel
    realign();

    // Updated sensors readings to display actual values at start
    updateSensorReadings();

    // Run timer that will be updating sensor readings
    mUpdateSensorReadingsTimer.setParent(this);
    connect(&mUpdateSensorReadingsTimer, SIGNAL(timeout()), this, SLOT(updateSensorReadings()));
    mUpdateSensorReadingsTimer.start(settings().value("updateInterval").toInt() * 1000);

    // Run timer that will be showin warning
    mWarningAboutHighTemperatureTimer.setParent(this);
    connect(&mWarningAboutHighTemperatureTimer, SIGNAL(timeout()), this,
            SLOT(warningAboutHighTemperature()));
    if (settings().value("warningAboutHighTemperature").toBool())
    {
        mWarningAboutHighTemperatureTimer.start(mWarningAboutHighTemperatureTimerFreq);
    }
}

RazorSensors::~RazorSensors()
{
}

void RazorSensors::updateSensorReadings()
{
    QString tooltip;

    double critTemp = 0;
    double maxTemp = 0;
    double minTemp = 0;
    double curTemp = 0;
    bool highTemperature = false;

    // Iterator for temperature progress bars
    std::vector<QProgressBar*>::iterator temperatureProgressBarsIt =
        mTemperatureProgressBars.begin();

    for (unsigned int i = 0; i < mDetectedChips.size(); ++i)
    {
        const std::vector<Feature>& features = mDetectedChips[i].getFeatures();

        for (unsigned int j = 0; j < features.size(); ++j)
        {
            if (features[j].getType() == SENSORS_FEATURE_TEMP)
            {
                tooltip = QString::fromStdString(features[j].getLabel()) + " (" + QChar(0x00B0);

                if (settings().value("useFahrenheitScale").toBool())
                {
                    critTemp = celsiusToFahrenheit(
                        features[j].getValue(SENSORS_SUBFEATURE_TEMP_CRIT));
                    maxTemp = celsiusToFahrenheit(
                        features[j].getValue(SENSORS_SUBFEATURE_TEMP_MAX));
                    minTemp = celsiusToFahrenheit(
                        features[j].getValue(SENSORS_SUBFEATURE_TEMP_MIN));
                    curTemp = celsiusToFahrenheit(
                        features[j].getValue(SENSORS_SUBFEATURE_TEMP_INPUT));

                    tooltip += "F)";
                }
                else
                {
                    critTemp = features[j].getValue(SENSORS_SUBFEATURE_TEMP_CRIT);
                    maxTemp = features[j].getValue(SENSORS_SUBFEATURE_TEMP_MAX);
                    minTemp = features[j].getValue(SENSORS_SUBFEATURE_TEMP_MIN);
                    curTemp = features[j].getValue(SENSORS_SUBFEATURE_TEMP_INPUT);

                    tooltip += "C)";
                }


                // Check if temperature is too high
                if (curTemp >= maxTemp)
                {
                    if (settings().value("warningAboutHighTemperature").toBool())
                    {
                        // Add current progress bar to the "warning container"
                        mHighTemperatureProgressBars.insert(*temperatureProgressBarsIt);
                    }

                    highTemperature = true;
                }
                else
                {
                    mHighTemperatureProgressBars.erase(*temperatureProgressBarsIt);

                    highTemperature = false;
                }

                // Set maximum temperature
                (*temperatureProgressBarsIt)->setMaximum(critTemp);
                // Set minimum temperature
                (*temperatureProgressBarsIt)->setMinimum(minTemp);
                // Set current temperature
                (*temperatureProgressBarsIt)->setValue(curTemp);

                tooltip += "<br><br>Crit: ";
                tooltip += QString::number((*temperatureProgressBarsIt)->maximum());
                tooltip += "<br>Max: ";
                tooltip += QString::number(int(maxTemp));
                tooltip += "<br>Cur: ";

                // Mark high temperature in the tooltip
                if (highTemperature)
                {
                    tooltip += "<span style=\"font-size:8pt; font-weight:600; color:#FF0000;\">";
                    tooltip += QString::number((*temperatureProgressBarsIt)->value());
                    tooltip += " !</span>";
                }
                else
                {
                    tooltip += QString::number((*temperatureProgressBarsIt)->value());
                }

                tooltip += "<br>Min: ";
                tooltip += QString::number((*temperatureProgressBarsIt)->minimum());
                (*temperatureProgressBarsIt)->setToolTip(tooltip);

                // Go to the next temperature progress bar
                ++temperatureProgressBarsIt;
            }
        }
    }

    update();
}

void RazorSensors::warningAboutHighTemperature()
{
    // Iterator for temperature progress bars
    std::set<QProgressBar*>::iterator temperatureProgressBarsIt =
        mHighTemperatureProgressBars.begin();

    int curValue;
    int maxValue;

    for (; temperatureProgressBarsIt != mHighTemperatureProgressBars.end();
         ++temperatureProgressBarsIt)
    {
        curValue = (*temperatureProgressBarsIt)->value();
        maxValue = (*temperatureProgressBarsIt)->maximum();

        if (maxValue > curValue)
        {
            (*temperatureProgressBarsIt)->setValue(maxValue);
        }
        else
        {
            (*temperatureProgressBarsIt)->setValue((*temperatureProgressBarsIt)->minimum());
        }

    }
    update();
}

void RazorSensors::showConfigureDialog()
{
    RazorSensorsConfiguration *confWindow =
            this->findChild<RazorSensorsConfiguration*>("RazorSensorsConfigurationWindow");

    if (!confWindow)
    {
        confWindow = new RazorSensorsConfiguration(settings(), this);
    }

    confWindow->show();
    confWindow->raise();
    confWindow->activateWindow();
}

void RazorSensors::settingsChanged()
{
    mUpdateSensorReadingsTimer.setInterval(settings().value("updateInterval").toInt() * 1000);

    for (unsigned int i = 0; i < mTemperatureProgressBars.size(); ++i)
    {
        if (panel()->isHorizontal())
        {
            mTemperatureProgressBars[i]->setFixedWidth(settings().value("tempBarWidth").toInt());
        }
        else
        {
            mTemperatureProgressBars[i]->setFixedHeight(settings().value("tempBarWidth").toInt());
        }
    }

    // Iterator for temperature progress bars
    std::vector<QProgressBar*>::iterator temperatureProgressBarsIt =
        mTemperatureProgressBars.begin();

    settings().beginGroup("chips");

    for (unsigned int i = 0; i < mDetectedChips.size(); ++i)
    {
        settings().beginGroup(QString::fromStdString(mDetectedChips[i].getName()));
        const std::vector<Feature>& features = mDetectedChips[i].getFeatures();

        for (unsigned int j = 0; j < features.size(); ++j)
        {
            if (features[j].getType() == SENSORS_FEATURE_TEMP)
            {
                settings().beginGroup(QString::fromStdString(features[j].getLabel()));

                if (settings().value("enabled").toBool())
                {
                    (*temperatureProgressBarsIt)->show();
                }
                else
                {
                    (*temperatureProgressBarsIt)->hide();
                }

                QPalette pal = (*temperatureProgressBarsIt)->palette();
                QColor color(settings().value("color").toString());
                pal.setColor(QPalette::Active, QPalette::Highlight, color);
                pal.setColor(QPalette::Inactive, QPalette::Highlight, color);
                (*temperatureProgressBarsIt)->setPalette(pal);

                settings().endGroup();

                // Go to the next temperature progress bar
                ++temperatureProgressBarsIt;
            }
        }

        settings().endGroup();
    }

    settings().endGroup();


    if (settings().value("warningAboutHighTemperature").toBool())
    {
        // Update sensors readings to get the list of high temperature progress bars
        updateSensorReadings();

        mWarningAboutHighTemperatureTimer.start(mWarningAboutHighTemperatureTimerFreq);
    }
    else if (mWarningAboutHighTemperatureTimer.isActive())
    {
        mWarningAboutHighTemperatureTimer.stop();

        // Update sensors readings to set progress bar values to "normal" height
        updateSensorReadings();
    }

    update();
}

void RazorSensors::realign()
{
    // Default values for RazorPanel::PositionBottom or RazorPanel::PositionTop
    Qt::Orientation cur_orient = Qt::Vertical;
    Qt::LayoutDirection cur_layout_dir = Qt::LeftToRight;

    switch (panel()->position())
    {
    case RazorPanel::PositionLeft:
        cur_orient = Qt::Horizontal;
        break;
    case RazorPanel::PositionRight:
        cur_orient = Qt::Horizontal;
        cur_layout_dir = Qt::RightToLeft;
        break;
    default:
        break;
    }

    for (unsigned int i = 0; i < mTemperatureProgressBars.size(); ++i)
    {
        mTemperatureProgressBars[i]->setOrientation(cur_orient);
        mTemperatureProgressBars[i]->setLayoutDirection(cur_layout_dir);

        if (panel()->isHorizontal())
        {
            mTemperatureProgressBars[i]->setFixedWidth(settings().value("tempBarWidth").toInt());
            mTemperatureProgressBars[i]->setFixedHeight(QWIDGETSIZE_MAX);
        }
        else
        {
            mTemperatureProgressBars[i]->setFixedHeight(settings().value("tempBarWidth").toInt());
            mTemperatureProgressBars[i]->setFixedWidth(QWIDGETSIZE_MAX);
        }
    }
}

double RazorSensors::celsiusToFahrenheit(double celsius)
{
    // Fahrenheit = 32 * (9/5) * Celsius
    return 32 + 1.8 * celsius;
}

void RazorSensors::initDefaultSettings()
{
    if (!settings().contains("updateInterval"))
    {
        settings().setValue("updateInterval", 1);
    }

    if (!settings().contains("tempBarWidth"))
    {
        settings().setValue("tempBarWidth", 8);
    }

    if (!settings().contains("useFahrenheitScale"))
    {
        settings().setValue("useFahrenheitScale", false);
    }

    settings().beginGroup("chips");

    // Initialize default sensors settings
    for (unsigned int i = 0; i < mDetectedChips.size(); ++i)
    {
        settings().beginGroup(QString::fromStdString(mDetectedChips[i].getName()));
        const std::vector<Feature>& features = mDetectedChips[i].getFeatures();

        for (unsigned int j = 0; j < features.size(); ++j)
        {
            if (features[j].getType() == SENSORS_FEATURE_TEMP)
            {
                settings().beginGroup(QString::fromStdString(features[j].getLabel()));
                if (!settings().contains("enabled"))
                {
                    settings().setValue("enabled", true);
                }

                if (!settings().contains("color"))
                {
                    // This is the default from QtDesigner
                    settings().setValue("color", QColor(qRgb(98, 140, 178)).name());
                }
                settings().endGroup();
            }
        }
        settings().endGroup();
    }

    settings().endGroup();

    if (!settings().contains("warningAboutHighTemperature"))
    {
        settings().setValue("warningAboutHighTemperature", true);
    }
}
