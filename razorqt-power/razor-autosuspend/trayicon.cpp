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
#include <QAction>
#include <QCoreApplication>
#include <QMenu>
#include <QDebug>
#include <QDBusInterface>
#include <QDBusReply>
#include "razorqt/razornotification.h"

TrayIcon::TrayIcon(QWidget *parent) : QSystemTrayIcon(parent),
    razorPower(),
    settings("razor-autosuspend"),
    razorNotification(tr("Power low"), this),
    actionTime()
{
    makeContextMenu();
    setupBatteryStatusIcons();
    razorNotification.setIcon("razor-autosuspend");
    razorNotification.setUrgencyHint(RazorNotification::UrgencyCritical);
    connect(&lid, SIGNAL(changed(bool)), this, SLOT(lidChanged(bool)));
    connect(&battery, SIGNAL(batteryChanged()), this, SLOT(batteryChanged()));
    connect(&globalSettings, SIGNAL(iconThemeChanged()), this, SLOT(setupBatteryStatusIcons()));
}

TrayIcon::~TrayIcon()
{
}


void TrayIcon::makeContextMenu()
{
    if (contextMenu() == 0)
    {
        setContextMenu(new QMenu());
    }
    else
    {
        contextMenu()->clear();
    }

    QAction* settingsAction = contextMenu()->addAction(tr("Settings..."));
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(editSettings()));
    QAction* exitAction = contextMenu()->addAction(tr("Exit"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(exitAutoSuspender()));
}

void TrayIcon::exitAutoSuspender()
{
    QCoreApplication::exit(0);
}

void TrayIcon::lidChanged(bool closed)
{
    if (closed)
    {
        doAction(settings.value(LIDCLOSEDACTION_KEY).toInt());
    }
}

void TrayIcon::batteryChanged()
{
    qDebug() <<  "onBattery: "  << battery.onBattery() <<
                 "chargeLevel:" << battery.chargeLevel() <<
                 "powerlow:"    << battery.powerLow() <<
                 "actionTime:"  << actionTime;

    setStatusIcon();

    if (battery.powerLow() && actionTime.isNull() && powerLowAction() > 0)
    {
        int warningTimeMsecs = settings.value(POWERLOWWARNING_KEY, 30).toInt()*1000;
        actionTime = QTime::currentTime().addMSecs(warningTimeMsecs);
        startTimer(100);
        // From here everything is handled by timerEvent below
    }
}


void TrayIcon::timerEvent(QTimerEvent *event)
{
    if (actionTime.isNull() || powerLowAction() == 0 || ! battery.powerLow())
    {
            killTimer(event->timerId());
            actionTime = QTime();
    }
    else if (QTime::currentTime().msecsTo(actionTime) > 0)
    {
        QString notificationMsg = powerLowAction() == SLEEP ? tr("Sleeping in %1 seconds") : tr("Hibernating in %1 seconds");
        razorNotification.setBody(notificationMsg.arg(QTime::currentTime().msecsTo(actionTime)/1000));
        razorNotification.update();
    }
    else
    {
        doAction(powerLowAction());
        actionTime = QTime();
        killTimer(event->timerId());
    }
}

void TrayIcon::doAction(int action)
{
    switch (action)
    {
    case SLEEP:
        razorPower.suspend();
        break;
    case HIBERNATE:
        razorPower.hibernate();
        break;
    }
}




void TrayIcon::editSettings()
{
    SettingsDialog().exec();
}

int TrayIcon::powerLowAction()
{
    return settings.value(POWERLOWACTION_KEY).toInt();
}

void TrayIcon::setupBatteryStatusIcons()
{
    // TODO: These are the iconnames in oxygen-theme. Gnome icons have other names (sigh).
    QString dechargingIconNames[6] =
        {"battery-low", "battery-caution", "battery-040", "battery-060", "battery-080", "battery-100"};
    QString chargingIconNames[6] =
        {"battery-charging-low", "battery-charging-caution", "battery-charging-040", "battery-charging-060", "battery-charging-080", "battery-charging"};

    m_hasBatteryStatusIcons = true;
    for (int i = 0; i < 6; i++)
    {
        qDebug() << dechargingIconNames[i] << chargingIconNames[i];

        if (! (QIcon::hasThemeIcon(dechargingIconNames[i]) && QIcon::hasThemeIcon(chargingIconNames[i])))
        {
            m_hasBatteryStatusIcons = false;
            break;
        }
        else
        {
            m_chargingBatteryStatusIcons[i] = QIcon::fromTheme(chargingIconNames[i]);
            m_dechargingBatteryStatusIcons[i] = QIcon::fromTheme(dechargingIconNames[i]);
        }
    }
    setStatusIcon();
}

void TrayIcon::setStatusIcon()
{
    if (m_hasBatteryStatusIcons)
    {
        int index = battery.chargeLevel() < 10 ? 0 :
                    battery.chargeLevel() < 30 ? 1 :
                    battery.chargeLevel() < 50 ? 2 :
                    battery.chargeLevel() < 70 ? 3 :
                    battery.chargeLevel() < 90 ? 4 :
                                                 5 ;

        QIcon icon = battery.onBattery() ? m_dechargingBatteryStatusIcons[index] : m_chargingBatteryStatusIcons[index];
        setIcon(icon);
        qDebug() << "chargeLevel: " <<  battery.chargeLevel() << " -> index: " << index << ", icon name:" << icon.name();
    }
    else
    {
        setIcon(QIcon(":icons/razor-autosuspend.svg"));
    }

    QString tooltip(battery.onBattery() ? "Decharging: %1%" : "Charging: %1%");
    setToolTip(tooltip.arg(QString::number(battery.chargeLevel(), 'f', 1)));
}
