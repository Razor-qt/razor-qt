/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright (C) 2012  Alec Moskvin <alecm@gmx.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "razornotification.h"
#include "razornotification_p.h"
#include <QtGui/QMessageBox>
#include <QDebug>

RazorNotification::~RazorNotification()
{
    Q_D(RazorNotification);
    delete d;
}

void RazorNotification::notify(const QString &summary, const QString &body, const QString &iconName)
{
    Q_D(RazorNotification);
    d->notify(summary, body, iconName);
}

void RazorNotification::close()
{
    Q_D(RazorNotification);
    d->close();
}

RazorNotificationPrivate::RazorNotificationPrivate(RazorNotification *parent) :
    q_ptr(parent)
{
    mInterface = new OrgFreedesktopNotificationsInterface("org.freedesktop.Notifications",
                                                          "/org/freedesktop/Notifications",
                                                          QDBusConnection::sessionBus(), 0);
    connect(mInterface, SIGNAL(NotificationClosed(uint, uint)), this, SLOT(notificationClosed(uint,uint)));
}

RazorNotificationPrivate::~RazorNotificationPrivate()
{
}

void RazorNotificationPrivate::notify(const QString& summary, const QString& body, const QString& iconName)
{
    QDBusPendingReply<uint> reply = mInterface->Notify(qAppName(), mId, iconName, summary, body, QStringList(), QVariantMap(), -1);
    reply.waitForFinished();
    if (reply.isError())
    {
        QMessageBox::information(0, tr("Notifications Fallback"), summary + " \n\n " + body);
    }
    mId = reply.value();
}

void RazorNotificationPrivate::close()
{
    mInterface->CloseNotification(mId);
    mId = 0;
}

void RazorNotificationPrivate::notificationClosed(uint id, uint reason)
{
    Q_Q(RazorNotification);
    if (id != 0 && id == mId)
    {
        mId = 0;
    }
    emit q->notificationClosed(RazorNotification::CloseReason(reason));
}

RazorNotification::RazorNotification(QObject* parent) :
    QObject(parent),
    d_ptr(new RazorNotificationPrivate(this))
{
}
