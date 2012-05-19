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

#ifndef RAZORSENSORS_H
#define RAZORSENSORS_H

#include "../panel/razorpanelplugin.h"
#include "sensors.h"
#include <QtCore/QTimer>
#include <QtGui/QProgressBar>
#include <sensors/sensors.h>

class RazorSensors : public RazorPanelPlugin
{
    Q_OBJECT
public:
    RazorSensors(const RazorPanelPluginStartInfo* startInfo, QWidget* parent = 0);
    ~RazorSensors();

    virtual RazorPanelPlugin::Flags flags() const { return PreferRightAlignment  | HaveConfigDialog ; }

public slots:
    void updateSensorReadings();

private slots:
    virtual void showConfigureDialog();
    virtual void settingsChanged();
    virtual void realign();

private:
    QTimer *mUpdateSensorReadingsTimer;
    Sensors mSensors;
    std::vector<Chip> mDetectedChips;
    std::vector<QProgressBar*> mTemperatureProgressBars;
    double celsiusToFahrenheit(double celsius);
    void initDefaultSettings();
};

EXPORT_RAZOR_PANEL_PLUGIN_H

#endif // RAZORSENSORS_H
