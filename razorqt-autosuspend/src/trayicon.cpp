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
#include "qtxdg/qiconfix/qiconloader_p.h"
#include "razorqt/razorsettings.h"
#include "battery.h"
#include "../config/constants.h"
#include <QIcon>
#include <QDebug>
#include <math.h>
#include <QtGui/qicon.h>
#include <sys/param.h>

TrayIcon::TrayIcon(Battery* battery, QObject *parent) : 
    QSystemTrayIcon(parent), 
        battery(battery), 
        m_Settings("razor-autosuspend")
{
    setUpstatusIcons();

    connect(battery, SIGNAL(batteryChanged()), this, SLOT(update()));
    connect(RazorSettings::globalSettings(), SIGNAL(iconThemeChanged()), this, SLOT(iconThemeChanged()));
    connect(&m_Settings, SIGNAL(settingsChanged()), this, SLOT(settingsChanged()));
    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(showStatus(QSystemTrayIcon::ActivationReason)));

    checkThemeStatusIcons(); 
    update(); 
    setVisible(m_Settings.value(SHOWTRAYICON_KEY, true).toBool());
}

TrayIcon::~TrayIcon()
{
}

void TrayIcon::update()
{
    updateStatusIcon();
    updateToolTip();
    batteryInfo.updateInfo(battery);
}


void TrayIcon::updateStatusIcon()
{
    if (m_Settings.value(USETHEMEICONS_KEY, true).toBool() && themeHasStatusIcons)
    {
        QString iconName;
        bool charging = ! battery->decharging();

        if (QIcon::themeName() == "oxygen")
        {
            if (battery->chargeLevel() < 20)        iconName = charging ? "battery-charging-low" : "battery-low";
            else if (battery->chargeLevel() < 40)   iconName = charging ? "battery-charging-caution" : "battery-caution";
            else if (battery->chargeLevel() < 60)   iconName = charging ? "battery-charging-040" : "battery-040";
            else if (battery->chargeLevel() < 80)   iconName = charging ? "battery-charging-060" : "battery-060";
            else if (battery->chargeLevel() < 99.5) iconName = charging ? "battery-charging-080" : "battery-080";
            else                                  iconName = charging ? "battery-charging" : "battery-100";
        }
        else // For all themes but 'oxygen' we follow freedesktop's battery status icon name standard 
             // (http://standards.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html) _with_ the changes proposed in
             // https://bugs.freedesktop.org/show_bug.cgi?id=41458 (we assume that this patch will be accepted)
        {
            if (battery->chargeLevel() <  1 && !charging) iconName = "battery-empty" ;
            else if (battery->chargeLevel() < 20)         iconName = charging ? "battery-caution-charging" : "battery-caution";
            else if (battery->chargeLevel() < 40)         iconName = charging ? "battery-low-charging" : "battery-low";
            else if (battery->chargeLevel() < 60)         iconName = charging ? "battery-good-charging" : "battery-good";
            else                                          iconName = charging ? "battery-full-charging" : "battery-full";
        }
        
        qDebug() << "ChargeLevel" << battery->chargeLevel() 
                 << "- getting icon:"  << iconName 
                 << "from" << QIcon::themeName() << "theme";
        setIcon(QIcon::fromTheme(iconName));
    }
    else
    {
        int chargeLevel0_10 = round(battery->chargeLevel()/10);

        if (battery->decharging())
        {
            setIcon(statusIconsDecharging[chargeLevel0_10]);
        }
        else 
        {
            setIcon(statusIconsCharging[chargeLevel0_10]);
        }
    }
}

void TrayIcon::updateToolTip()
{
    QString toolTip = battery->stateAsString();

    if (battery->state() == 1 || battery->state() == 2)
    {
        toolTip = toolTip + QString(" - %1 %").arg(battery->chargeLevel(), 0, 'f', 1);
    }
    setToolTip(toolTip);
}



void TrayIcon::checkThemeStatusIcons()
{
    themeHasStatusIcons = true; 
    if ("oxygen" != QIcon::themeName())
    {
        // We know what icons the oxygen theme contains, so with oxygen we're good
        // but we don't know that all icon themes have those that the freedesktop
        // standard prescribes. If the current theme doesn't, we will fall back to 
        // the built in status icons.

        static QStringList statusIconNames = QStringList() 
            <<  "battery-empty" 
            << "battery-caution"           << "battery-low"          << "battery-good"          << "battery-full"
            <<  "battery-caution-charging" << "battery-low-charging" << "battery-good-charging" << "battery-full-charging";

        foreach (QString statusIconName, statusIconNames) 
        {
            if (! QIcon::hasThemeIcon(statusIconName))
            {
                themeHasStatusIcons = false;
                break;
            }
        }
    }
}

void TrayIcon::iconThemeChanged()
{
    checkThemeStatusIcons();
    updateStatusIcon();
}

void TrayIcon::settingsChanged()
{
    updateStatusIcon();
    setVisible(m_Settings.value(SHOWTRAYICON_KEY, true).toBool()); 
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

