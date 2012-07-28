/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Christian Surlykke <christian@surlykke.dk>
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

#include "trayicon.h"

#include <QIcon>
#include <QDebug>
#include <math.h>

TrayIcon::TrayIcon(QWidget *parent) : QSystemTrayIcon(parent)
{
    qDebug() << "themeName: " << QIcon::themeName();
    setUpstatusIcons();
    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(showStatus(QSystemTrayIcon::ActivationReason)));
}

TrayIcon::~TrayIcon()
{
}

void TrayIcon::setStatus(double level, bool onBattery, QVariantMap batteryProperties)
{
    int levelAsNumberBetween0and10 = round(level/10);

    qDebug() << "Level: " << level << levelAsNumberBetween0and10;

    uint state = batteryProperties.value("State", 0).toUInt();
    QString toolTip = BatteryInfo::state2string(state);
    if (state == 1 || state == 2)
    {
        toolTip = toolTip + QString(" - %1 %").arg(batteryProperties.value("Percentage").toDouble(), 0, 'f', 1);
    }
    setToolTip(toolTip);

    if (state == 1 || state == 4)
    {
        setIcon(statusIconsCharging[levelAsNumberBetween0and10]);
    }
    else
    {
        setIcon(statusIconsDecharging[levelAsNumberBetween0and10]);
    }

    this->batteryProperties = batteryProperties;
    batteryInfo.updateInfo(batteryProperties);
}

void TrayIcon::setUpstatusIcons()
{
    statusIconsCharging[0] = QIcon(":icons/battery-charging-000.svg");
    statusIconsCharging[1] = QIcon(":icons/battery-charging-010.svg");
    statusIconsCharging[2] = QIcon(":icons/battery-charging-020.svg");
    statusIconsCharging[3] = QIcon(":icons/battery-charging-030.svg");
    statusIconsCharging[4] = QIcon(":icons/battery-charging-040.svg");
    statusIconsCharging[5] = QIcon(":icons/battery-charging-050.svg");
    statusIconsCharging[6] = QIcon(":icons/battery-charging-060.svg");
    statusIconsCharging[7] = QIcon(":icons/battery-charging-070.svg");
    statusIconsCharging[8] = QIcon(":icons/battery-charging-080.svg");
    statusIconsCharging[9] = QIcon(":icons/battery-charging-090.svg");
    statusIconsCharging[10] = QIcon(":icons/battery-charging-100.svg");

    statusIconsDecharging[0] = QIcon(":icons/battery-000.svg");
    statusIconsDecharging[1] = QIcon(":icons/battery-010.svg");
    statusIconsDecharging[2] = QIcon(":icons/battery-020.svg");
    statusIconsDecharging[3] = QIcon(":icons/battery-030.svg");
    statusIconsDecharging[4] = QIcon(":icons/battery-040.svg");
    statusIconsDecharging[5] = QIcon(":icons/battery-050.svg");
    statusIconsDecharging[6] = QIcon(":icons/battery-060.svg");
    statusIconsDecharging[7] = QIcon(":icons/battery-070.svg");
    statusIconsDecharging[8] = QIcon(":icons/battery-080.svg");
    statusIconsDecharging[9] = QIcon(":icons/battery-090.svg");
    statusIconsDecharging[10] = QIcon(":icons/battery-100.svg");
}

void TrayIcon::showStatus(ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger)
    {
        if (batteryInfo.isVisible())
        {
            batteryInfo.close();
        }
        else
        {
            batteryInfo.open();
        }
    }
}
