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
    mRazorNotification(tr("Power low"), this),
    mActionTime(),
    mSettings("razor-autosuspend")
{
    mRazorNotification.setIcon("razor-autosuspend");
    mRazorNotification.setUrgencyHint(RazorNotification::UrgencyCritical);
    mRazorNotification.setTimeout(2000);
    mBattery = new Battery(this);
    new TrayIcon(mBattery, this);
    
    connect(&mLid, SIGNAL(changed(bool)), this, SLOT(lidChanged(bool)));
    connect(mBattery, SIGNAL(batteryChanged()), this, SLOT(batteryChanged()));
}

RazorAutosuspendd::~RazorAutosuspendd()
{
}

void RazorAutosuspendd::lidChanged(bool closed)
{
    if (closed)
    {
        doAction(mSettings.value(LIDCLOSEDACTION_KEY).toInt());
    }
}

void RazorAutosuspendd::batteryChanged()
{
    qDebug() <<  "BatteryChanged"
             <<  "discharging:"  << mBattery->discharging() 
             << "chargeLevel:" << mBattery->chargeLevel() 
             << "powerlow:"    << mBattery->powerLow() 
             << "actionTime:"  << mActionTime;

    if (mBattery->powerLow() && mActionTime.isNull() && powerLowAction() > 0)
    {
        int warningTimeMsecs = mSettings.value(POWERLOWWARNING_KEY, 30).toInt()*1000;
        mActionTime = QTime::currentTime().addMSecs(warningTimeMsecs);
        startTimer(100);
        // From here everything is handled by timerEvent below
    }
}

void RazorAutosuspendd::timerEvent(QTimerEvent *event)
{
    if (mActionTime.isNull() || powerLowAction() == 0 || ! mBattery->powerLow())
    {
            killTimer(event->timerId());
            mActionTime = QTime();
    }
    else if (QTime::currentTime().msecsTo(mActionTime) > 0)
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

        mRazorNotification.setBody(notificationMsg.arg(QTime::currentTime().msecsTo(mActionTime)/1000));
        mRazorNotification.update();
    }
    else
    {
        doAction(powerLowAction());
        mActionTime = QTime();
        killTimer(event->timerId());
    }
}

void RazorAutosuspendd::doAction(int action)
{
    switch (action)
    {
    case SLEEP:
        mRazorPower.suspend();
        break;
    case HIBERNATE:
        mRazorPower.hibernate();
        break;
    case POWEROFF:
        mRazorPower.shutdown();
        break;
    }
}

int RazorAutosuspendd::powerLowAction()
{
    return mSettings.value(POWERLOWACTION_KEY).toInt();
}

