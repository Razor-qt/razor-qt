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

TrayIcon::TrayIcon(QWidget *parent) : QSystemTrayIcon(parent),
    powerInterface("org.freedesktop.UPower", "/org/freedesktop/UPower", QDBusConnection::systemBus(), this),
    settings("razor-autosuspend")

{
    setIcon(QIcon(":icons/icon.svg"));
    makeContextMenu();

    connect(&powerInterface, SIGNAL(Changed()), this, SLOT(upowerPropertyChanged()));
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

    QAction* settingsAction = contextMenu()->addAction("Settings...");
    connect(settingsAction, SIGNAL(triggered()), &settingsDialog, SLOT(exec()));
    QAction* exitAction = contextMenu()->addAction("Exit");
    connect(exitAction, SIGNAL(triggered()), this, SLOT(exitAutoSuspender()));
}

void TrayIcon::exitAutoSuspender()
{
    QCoreApplication::exit(0);
}

void TrayIcon::upowerPropertyChanged()
{
    if (powerInterface.lidIsClosed())
    {
        int lidClosedAction = settings.value(LIDCLOSEDACTION_KEY).toInt();
        switch (lidClosedAction)
        {
        case SLEEP:
            powerInterface.Suspend();
            break;
        case HIBERNATE:
            powerInterface.Hibernate();
            break;
        }
    }
    else if (powerInterface.onLowBattery())
    {
        int lowBatteryAction = settings.value(POWERLOWACTION_KEY).toInt();
        switch (lowBatteryAction) {
        case SLEEP:
            powerInterface.Suspend();
            break;
        case HIBERNATE:
            powerInterface.Hibernate();
            break;
        }
    }

}
