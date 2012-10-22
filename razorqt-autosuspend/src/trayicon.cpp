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
        mBattery(battery), 
        mSettings("razor-autosuspend")
{
    setUpstatusIcons();

    connect(mBattery, SIGNAL(batteryChanged()), this, SLOT(update()));
    connect(RazorSettings::globalSettings(), SIGNAL(iconThemeChanged()), this, SLOT(iconThemeChanged()));
    connect(&mSettings, SIGNAL(settingsChanged()), this, SLOT(settingsChanged()));
    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(showStatus(QSystemTrayIcon::ActivationReason)));

    checkThemeStatusIcons(); 
    update(); 
    setVisible(mSettings.value(SHOWTRAYICON_KEY, true).toBool());
}

TrayIcon::~TrayIcon()
{
}

void TrayIcon::update()
{
    updateStatusIcon();
    updateToolTip();
    mBatteryInfo.updateInfo(mBattery);
}


void TrayIcon::updateStatusIcon()
{
    if (mSettings.value(USETHEMEICONS_KEY, true).toBool() && mThemeHasStatusIcons)
    {
        QString iconName;
        bool charging = ! mBattery->decharging();

        if (QIcon::themeName() == "oxygen")
        {
            if (mBattery->chargeLevel() < 20)        iconName = charging ? "battery-charging-low" : "battery-low";
            else if (mBattery->chargeLevel() < 40)   iconName = charging ? "battery-charging-caution" : "battery-caution";
            else if (mBattery->chargeLevel() < 60)   iconName = charging ? "battery-charging-040" : "battery-040";
            else if (mBattery->chargeLevel() < 80)   iconName = charging ? "battery-charging-060" : "battery-060";
            else if (mBattery->chargeLevel() < 99.5) iconName = charging ? "battery-charging-080" : "battery-080";
            else                                  iconName = charging ? "battery-charging" : "battery-100";
        }
        else // For all themes but 'oxygen' we follow freedesktop's battery status icon name standard 
             // (http://standards.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html) _with_ the changes proposed in
             // https://bugs.freedesktop.org/show_bug.cgi?id=41458 (we assume that this patch will be accepted)
        {
            if (mBattery->chargeLevel() <  1 && !charging) iconName = "battery-empty" ;
            else if (mBattery->chargeLevel() < 20)         iconName = charging ? "battery-caution-charging" : "battery-caution";
            else if (mBattery->chargeLevel() < 40)         iconName = charging ? "battery-low-charging" : "battery-low";
            else if (mBattery->chargeLevel() < 60)         iconName = charging ? "battery-good-charging" : "battery-good";
            else                                          iconName = charging ? "battery-full-charging" : "battery-full";
        }
        
        qDebug() << "ChargeLevel" << mBattery->chargeLevel() 
                 << "- getting icon:"  << iconName 
                 << "from" << QIcon::themeName() << "theme";
        setIcon(QIcon::fromTheme(iconName));
    }
    else
    {
        int chargeLevel0_10 = round(mBattery->chargeLevel()/10);

        if (mBattery->decharging())
        {
            setIcon(mStatusIconsDecharging[chargeLevel0_10]);
        }
        else 
        {
            setIcon(mStatusIconsCharging[chargeLevel0_10]);
        }
    }
}

void TrayIcon::updateToolTip()
{
    QString toolTip = mBattery->stateAsString();

    if (mBattery->state() == 1 || mBattery->state() == 2)
    {
        toolTip = toolTip + QString(" - %1 %").arg(mBattery->chargeLevel(), 0, 'f', 1);
    }
    setToolTip(toolTip);
}



void TrayIcon::checkThemeStatusIcons()
{
    mThemeHasStatusIcons = true; 
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
                mThemeHasStatusIcons = false;
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
    setVisible(mSettings.value(SHOWTRAYICON_KEY, true).toBool()); 
}

void TrayIcon::setUpstatusIcons()
{
    mStatusIconsCharging[0] = QIcon(":icons/battery-charging-000.svg");
    mStatusIconsCharging[1] = QIcon(":icons/battery-charging-010.svg");
    mStatusIconsCharging[2] = QIcon(":icons/battery-charging-020.svg");
    mStatusIconsCharging[3] = QIcon(":icons/battery-charging-030.svg");
    mStatusIconsCharging[4] = QIcon(":icons/battery-charging-040.svg");
    mStatusIconsCharging[5] = QIcon(":icons/battery-charging-050.svg");
    mStatusIconsCharging[6] = QIcon(":icons/battery-charging-060.svg");
    mStatusIconsCharging[7] = QIcon(":icons/battery-charging-070.svg");
    mStatusIconsCharging[8] = QIcon(":icons/battery-charging-080.svg");
    mStatusIconsCharging[9] = QIcon(":icons/battery-charging-090.svg");
    mStatusIconsCharging[10] = QIcon(":icons/battery-charging-100.svg");

    mStatusIconsDecharging[0] = QIcon(":icons/battery-000.svg");
    mStatusIconsDecharging[1] = QIcon(":icons/battery-010.svg");
    mStatusIconsDecharging[2] = QIcon(":icons/battery-020.svg");
    mStatusIconsDecharging[3] = QIcon(":icons/battery-030.svg");
    mStatusIconsDecharging[4] = QIcon(":icons/battery-040.svg");
    mStatusIconsDecharging[5] = QIcon(":icons/battery-050.svg");
    mStatusIconsDecharging[6] = QIcon(":icons/battery-060.svg");
    mStatusIconsDecharging[7] = QIcon(":icons/battery-070.svg");
    mStatusIconsDecharging[8] = QIcon(":icons/battery-080.svg");
    mStatusIconsDecharging[9] = QIcon(":icons/battery-090.svg");
    mStatusIconsDecharging[10] = QIcon(":icons/battery-100.svg");
}


void TrayIcon::showStatus(ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger)
    {
        if (mBatteryInfo.isVisible())
        {
            mBatteryInfo.close();
        }
        else
        {
            mBatteryInfo.open();
        }
    }
}

