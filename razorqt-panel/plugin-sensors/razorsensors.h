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

#include "sensors.h"
#include <QFrame>
#include <QtCore/QTimer>
#include <QProgressBar>
#include <sensors/sensors.h>
#include <set>

class ProgressBar: public QProgressBar
{
    Q_OBJECT
public:
    ProgressBar(QWidget *parent = 0);

    QSize sizeHint() const;
};


class QSettings;
class IRazorPanelPlugin;
class QBoxLayout;

class RazorSensors : public QFrame
{
    Q_OBJECT
public:
    RazorSensors(IRazorPanelPlugin *plugin, QWidget* parent = 0);
    ~RazorSensors();

    void settingsChanged();
    void realign();
public slots:
    void updateSensorReadings();
    void warningAboutHighTemperature();

private:
    IRazorPanelPlugin *mPlugin;
    QBoxLayout *mLayout;
    QTimer mUpdateSensorReadingsTimer;
    QTimer mWarningAboutHighTemperatureTimer;
    // How often warning time should fire in ms
    int mWarningAboutHighTemperatureTimerFreq;
    Sensors mSensors;
    std::vector<Chip> mDetectedChips;
    std::vector<ProgressBar*> mTemperatureProgressBars;
    // With set we can handle updates in very easy way :)
    std::set<ProgressBar*> mHighTemperatureProgressBars;
    double celsiusToFahrenheit(double celsius);
    void initDefaultSettings();
    QSettings *mSettings;
};


#endif // RAZORSENSORS_H
