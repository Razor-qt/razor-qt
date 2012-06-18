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

void TrayIcon::chargeLevelChanged(double newPercentage)
{
    Q_UNUSED(newPercentage)

    qDebug() << "charge:" << newPercentage << "actionTime:" << actionTime << "powerlow:" << battery.powerLow();

    if (battery.powerLow() && actionTime.isNull() && powerLowAction() > 0)
    {
        actionTime = QTime::currentTime().addMSecs(30000);
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
