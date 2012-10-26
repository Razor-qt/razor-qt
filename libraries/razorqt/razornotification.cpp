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

RazorNotification::RazorNotification(const QString& summary, QObject* parent) :
    QObject(parent),
    d_ptr(new RazorNotificationPrivate(summary, this))
{
}

RazorNotification::~RazorNotification()
{
    Q_D(RazorNotification);
    delete d;
}

void RazorNotification::update()
{
    Q_D(RazorNotification);
    d->update();
}

void RazorNotification::close()
{
    Q_D(RazorNotification);
    d->close();
}

void RazorNotification::setSummary(const QString& summary)
{
    Q_D(RazorNotification);
    d->mSummary = summary;
}

void RazorNotification::setBody(const QString& body)
{
    Q_D(RazorNotification);
    d->mBody = body;
}

void RazorNotification::setIcon(const QString& iconName)
{
    Q_D(RazorNotification);
    d->mIconName = iconName;
}

void RazorNotification::setActions(const QStringList& actions, int defaultAction)
{
    Q_D(RazorNotification);
    d->setActions(actions, defaultAction);
}

void RazorNotification::setTimeout(int timeout)
{
    Q_D(RazorNotification);
    d->mTimeout = timeout;
}

void RazorNotification::setHint(const QString& hintName, const QVariant& value)
{
    Q_D(RazorNotification);
    d->mHints.insert(hintName, value);
}

void RazorNotification::setUrgencyHint(Urgency urgency)
{
    Q_D(RazorNotification);
    d->mHints.insert("urgency", qvariant_cast<uchar>(urgency));
}

void RazorNotification::clearHints()
{
    Q_D(RazorNotification);
    d->mHints.clear();
}

QStringList RazorNotification::getCapabilities()
{
    Q_D(RazorNotification);
    return d->mInterface->GetCapabilities().value();
}

const RazorNotification::ServerInfo RazorNotification::serverInfo()
{
    Q_D(RazorNotification);
    return d->serverInfo();
}

void RazorNotification::notify(const QString& summary, const QString& body, const QString& iconName)
{
    RazorNotification notification(summary);
    notification.setBody(body);
    notification.setIcon(iconName);
    notification.update();
}

RazorNotificationPrivate::RazorNotificationPrivate(const QString& summary, RazorNotification* parent) :
    mId(0),
    mSummary(summary),
    mTimeout(-1),
    q_ptr(parent)
{
    mInterface = new OrgFreedesktopNotificationsInterface("org.freedesktop.Notifications",
                                                          "/org/freedesktop/Notifications",
                                                          QDBusConnection::sessionBus(), this);
    connect(mInterface, SIGNAL(NotificationClosed(uint, uint)), this, SLOT(notificationClosed(uint,uint)));
    connect(mInterface, SIGNAL(ActionInvoked(uint,QString)), this, SLOT(handleAction(uint,QString)));
}

RazorNotificationPrivate::~RazorNotificationPrivate()
{
}

void RazorNotificationPrivate::update()
{
    QDBusPendingReply<uint> reply = mInterface->Notify(qAppName(), mId, mIconName, mSummary, mBody, mActions, mHints, mTimeout);
    reply.waitForFinished();
    if (!reply.isError())
    {
        mId = reply.value();
    }
    else
    {
        if (mHints.contains("urgency") && mHints.value("urgency").toInt() != RazorNotification::UrgencyLow)
            QMessageBox::information(0, tr("Notifications Fallback"), mSummary + " \n\n " + mBody);
    }
}


void RazorNotificationPrivate::setActions(QStringList actions, int defaultAction)
{
    mActions.clear();
    mDefaultAction = defaultAction;
    for (int ix = 0; ix < actions.size(); ix++)
    {
        if (ix == defaultAction)
            mActions.append("default");
        else
            mActions.append(QString::number(ix));
        mActions.append(actions[ix]);
    }
}

const RazorNotification::ServerInfo RazorNotificationPrivate::serverInfo()
{
    RazorNotification::ServerInfo info;
    info.name = mInterface->GetServerInformation(info.vendor, info.version, info.specVersion);
    return info;
}

void RazorNotificationPrivate::handleAction(uint id, QString key)
{
    if (id != mId)
        return;

    Q_Q(RazorNotification);
    qDebug() << "action invoked:" << key;
    bool ok = true;
    int keyId;
    if (key == "default")
        keyId = mDefaultAction;
    else
        keyId = key.toInt(&ok);

    if (ok && key >= 0)
        emit q->actionActivated(keyId);
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
