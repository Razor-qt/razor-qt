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
        mUpdateSensorReadingsTimer(NULL)
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

                pg = new QProgressBar();

                // Hide progress bar if it is not enabled
                if (!settings().value("enabled").toBool())
                {
                    pg->hide();
                }

                pg->setToolTip(chipFeatureLabel);
                pg->setTextVisible(false);

                QPalette pal = pg->palette();
                pal.setColor(QPalette::Active, QPalette::Highlight,
                             QColor(settings().value("color").toString()));
                pg->setPalette(pal);

                mDetectedChipsProgressBars.push_back(pg);
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

    // Run timer that will be updating
    mUpdateSensorReadingsTimer = new QTimer(this);
    connect(mUpdateSensorReadingsTimer, SIGNAL(timeout()), this, SLOT(updateSensorReadings()));
    mUpdateSensorReadingsTimer->start(settings().value("updateInterval").toInt() * 1000);
}

RazorSensors::~RazorSensors()
{
}

void RazorSensors::updateSensorReadings()
{
    QString tooltip;

    double max_temp = 0;
    double min_temp = 0;
    double cur_temp = 0;

    for (unsigned int i = 0, index = 0; i < mDetectedChips.size(); ++i)
    {
        const std::vector<Feature>& features = mDetectedChips[i].getFeatures();

        for (unsigned int j = 0; j < features.size(); ++j, ++index)
        {
            if (features[j].getType() == SENSORS_FEATURE_TEMP)
            {
                tooltip = QString::fromStdString(features[j].getLabel()) + " (" + QChar(0x00B0);

                if (settings().value("useFahrenheitScale").toBool())
                {
                    max_temp = celsiusToFahrenheit(
                            features[index].getValue(SENSORS_SUBFEATURE_TEMP_MAX));
                    min_temp = celsiusToFahrenheit(
                            features[index].getValue(SENSORS_SUBFEATURE_TEMP_MIN));
                    cur_temp = celsiusToFahrenheit(
                        features[index].getValue(SENSORS_SUBFEATURE_TEMP_INPUT));

                    tooltip += "F)";
                }
                else
                {
                    max_temp = features[index].getValue(SENSORS_SUBFEATURE_TEMP_MAX);
                    min_temp = features[index].getValue(SENSORS_SUBFEATURE_TEMP_MIN);
                    cur_temp = features[index].getValue(SENSORS_SUBFEATURE_TEMP_INPUT);

                    tooltip += "C)";
                }

                // Get maximum temperature
                mDetectedChipsProgressBars[index]->setMaximum(max_temp);
                // Get minimum temperature
                mDetectedChipsProgressBars[index]->setMinimum(min_temp);
                // Get current temperature
                mDetectedChipsProgressBars[index]->setValue(cur_temp);

                tooltip += "<br><br>Max: ";
                tooltip += QString::number(mDetectedChipsProgressBars[index]->maximum());
                tooltip += "<br>Cur: ";
                tooltip += QString::number(mDetectedChipsProgressBars[index]->value());
                tooltip += "<br>Min: ";
                tooltip += QString::number(mDetectedChipsProgressBars[index]->minimum());
                mDetectedChipsProgressBars[index]->setToolTip(tooltip);
            }
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
    mUpdateSensorReadingsTimer->setInterval(settings().value("updateInterval").toInt() * 1000);

    for (unsigned int i = 0; i < mDetectedChipsProgressBars.size(); ++i)
    {
        if (panel()->isHorizontal())
        {
            mDetectedChipsProgressBars[i]->setFixedWidth(settings().value("tempBarWidth").toInt());
        }
        else
        {
            mDetectedChipsProgressBars[i]->setFixedHeight(settings().value("tempBarWidth").toInt());
        }
    }

    settings().beginGroup("chips");

    for (unsigned int i = 0, index = 0; i < mDetectedChips.size(); ++i)
    {
        settings().beginGroup(QString::fromStdString(mDetectedChips[i].getName()));
        const std::vector<Feature>& features = mDetectedChips[i].getFeatures();

        for (unsigned int j = 0; j < features.size(); ++j, ++index)
        {
            if (features[j].getType() == SENSORS_FEATURE_TEMP)
            {
                settings().beginGroup(QString::fromStdString(features[j].getLabel()));

                if (settings().value("enabled").toBool())
                {
                    mDetectedChipsProgressBars[index]->show();
                }
                else
                {
                    mDetectedChipsProgressBars[index]->hide();
                }

                QPalette pal = mDetectedChipsProgressBars[index]->palette();
                pal.setColor(QPalette::Active, QPalette::Highlight,
                             QColor(settings().value("color").toString()));
                mDetectedChipsProgressBars[index]->setPalette(pal);

                settings().endGroup();
            }
        }

        settings().endGroup();
    }

    settings().endGroup();

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

    for (unsigned int i = 0; i < mDetectedChipsProgressBars.size(); ++i)
    {
        mDetectedChipsProgressBars[i]->setOrientation(cur_orient);
        mDetectedChipsProgressBars[i]->setLayoutDirection(cur_layout_dir);

        if (panel()->isHorizontal())
        {
            mDetectedChipsProgressBars[i]->setFixedWidth(settings().value("tempBarWidth").toInt());
            mDetectedChipsProgressBars[i]->setFixedHeight(QWIDGETSIZE_MAX);
        }
        else
        {
            mDetectedChipsProgressBars[i]->setFixedHeight(settings().value("tempBarWidth").toInt());
            mDetectedChipsProgressBars[i]->setFixedWidth(QWIDGETSIZE_MAX);
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
}
