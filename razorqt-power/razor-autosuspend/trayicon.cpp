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
    razorPower(), settings("razor-autosuspend"), razorNotification("Power low", this), pendingAction(0), actionTime(), notificationMsg("")
{
    setIcon(QIcon(":icons/razor-autosuspend.svg"));
    makeContextMenu();
    razorNotification.setIcon("razor-autosuspend");
    razorNotification.setUrgencyHint(RazorNotification::UrgencyCritical);
    connect(&lid, SIGNAL(changed(bool)), this, SLOT(lidChanged(bool)));
    connect(&battery, SIGNAL(levelChanged(double)), this, SLOT(chargeLevelChanged(double)));
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
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(editSettings()));
    QAction* exitAction = contextMenu()->addAction("Exit");
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

void TrayIcon::chargeLevelChanged(double newPercentage)
{
    Q_UNUSED(newPercentage)

    qDebug() << "charge:" << newPercentage << "pendingAction:" << pendingAction << "powerlow:" << battery.powerLow();

    if (pendingAction != 0 && ! battery.powerLow())
    {
        pendingAction = 0;
    }
    else if (battery.powerLow() && pendingAction == 0)
    {
        pendingAction = settings.value(POWERLOWACTION_KEY).toInt();
        if (pendingAction > 0) {
            actionTime = QTime::currentTime().addMSecs(30000);
            if (pendingAction == SLEEP)
            {
                notificationMsg = "Sleeping in %1 seconds";
            }
            else if (pendingAction == HIBERNATE)
            {
                notificationMsg = "Hibernating in %1 seconds";
            }
            startTimer(100);
        }
    }
}


void TrayIcon::timerEvent(QTimerEvent *event)
{
    if (pendingAction > 0)
    {
        int secondsToAction = QTime::currentTime().msecsTo(actionTime)/1000;
        if (secondsToAction < 0)
        {
           doAction(pendingAction);
            pendingAction = 0;
        }
        else
        {
            razorNotification.setBody(notificationMsg.arg(QString::number(secondsToAction)));
            razorNotification.update();
        }
    }
    else {
        killTimer(event->timerId());
        razorNotification.close();
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
