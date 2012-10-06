/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
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
#include <QDebug>
#include <QTimerEvent>

#include "razorautosuspend.h"
#include "../config/constants.h"

RazorAutosuspendd::RazorAutosuspendd(QObject *parent) :
    QObject(parent),
    razorNotification(tr("Power low"), this),
    actionTime(),
    m_Settings("razor-autosuspend")
{
    razorNotification.setIcon("razor-autosuspend");
    razorNotification.setUrgencyHint(RazorNotification::UrgencyCritical);
    razorNotification.setTimeout(2000);
    trayIcon.setStatus(battery.chargeLevel(), battery.onBattery(), battery.properties());
    settingsChanged();
    connect(&lid, SIGNAL(changed(bool)), this, SLOT(lidChanged(bool)));
    connect(&battery, SIGNAL(batteryChanged()), this, SLOT(batteryChanged()));
    connect(&m_Settings, SIGNAL(settingsChanged()), this, SLOT(settingsChanged()));
}

RazorAutosuspendd::~RazorAutosuspendd()
{
}

void RazorAutosuspendd::lidChanged(bool closed)
{
    qDebug() << "LidChanged: " << closed;
    qDebug() << "Action: " << m_Settings.value(LIDCLOSEDACTION_KEY).toInt();
    if (closed)
    {
        doAction(m_Settings.value(LIDCLOSEDACTION_KEY).toInt());
    }
}

void RazorAutosuspendd::batteryChanged()
{
    qDebug() <<  "onBattery: "  << battery.onBattery() <<
                 "chargeLevel:" << battery.chargeLevel() <<
                 "powerlow:"    << battery.powerLow() <<
                 "actionTime:"  << actionTime;

    trayIcon.setStatus(battery.chargeLevel(), battery.onBattery(), battery.properties());

    if (battery.powerLow() && actionTime.isNull() && powerLowAction() > 0)
    {
        int warningTimeMsecs = m_Settings.value(POWERLOWWARNING_KEY, 30).toInt()*1000;
        actionTime = QTime::currentTime().addMSecs(warningTimeMsecs);
        startTimer(100);
        // From here everything is handled by timerEvent below
    }
}

void RazorAutosuspendd::settingsChanged()
{
    if (m_Settings.value(SHOWTRAYICON_KEY, true).toBool())
    {
        trayIcon.show();
    }
    else
    {
        trayIcon.hide();
    }
}

void RazorAutosuspendd::timerEvent(QTimerEvent *event)
{
    if (actionTime.isNull() || powerLowAction() == 0 || ! battery.powerLow())
    {
            killTimer(event->timerId());
            actionTime = QTime();
    }
    else if (QTime::currentTime().msecsTo(actionTime) > 0)
    {
        QString notificationMsg;
        switch (powerLowAction())
        {
        case SLEEP:
            notificationMsg = tr("Sleeping in %1 seconds");
            break;
        case HIBERNATE:
            notificationMsg = tr("Hibernating in %1 seconds");
            break;
        case POWEROFF:
            notificationMsg = tr("Shutting down in %1 seconds");
            break;
        }

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

void RazorAutosuspendd::doAction(int action)
{
    switch (action)
    {
    case SLEEP:
        razorPower.suspend();
        break;
    case HIBERNATE:
        razorPower.hibernate();
        break;
    case POWEROFF:
        razorPower.shutdown();
        break;
    }
}

int RazorAutosuspendd::powerLowAction()
{
    return m_Settings.value(POWERLOWACTION_KEY).toInt();
}

