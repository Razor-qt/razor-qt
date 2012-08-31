/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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

#include "notificationlayout.h"
#include <QtDebug>
#include <QtGui/QBrush>


NotificationLayout::NotificationLayout(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("NotificationLayout");

    // Hack to ensure the fully transparent background
    QPalette palette;
    palette.setBrush(QPalette::Base, Qt::NoBrush);
    setPalette(palette);
    // Required to display wallpaper
    setAttribute(Qt::WA_TranslucentBackground);

    m_layout = new QVBoxLayout(this);
    m_layout->setMargin(0);
    setLayout(m_layout);
}

void NotificationLayout::setSizes(int space, int width)
{
    m_layout->setSpacing(space);
    setMaximumWidth(width);
    setMinimumWidth(width);

    QHashIterator<uint, Notification*> it(m_notifications);
    while (it.hasNext())
    {
        it.next();
        it.value()->setMinimumWidth(width);
        it.value()->setMaximumWidth(width);
    }
}

void NotificationLayout::addNotification(uint id, const QString &application,
                                        const QString &summary, const QString &body,
                                        const QString &icon, int timeout,
                                        const QStringList& actions, const QVariantMap& hints)
{
//    qDebug() << "NotificationLayout::addNotification" << id << application << summary << body << icon << timeout;
    if (m_notifications.contains(id))
    {
        // TODO/FIXME: it can be deleted by timer in this block. Locking?
        Notification *n = m_notifications[id];
        n->setValues(application, summary, body, icon, timeout, actions, hints);
    }
    else
    {
        Notification *n = new Notification(application, summary, body, icon, timeout, actions, hints, this);

        // NOTE: it's hard to use == operator for Notification* in QList...
        QHashIterator<uint, Notification*> it(m_notifications);
        while (it.hasNext())
        {
            it.next();
            if (it.value()->application() == application
                    && it.value()->summary() == summary
                    && it.value()->body() == body)
            {
                qDebug() << "Notification app" << application << "summary" << summary << "is already registered but this request is not an update. Broken app?";
                delete n;
                return;
            }
        }

        connect(n, SIGNAL(timeout()), this, SLOT(removeNotificationTimeout()));
        connect(n, SIGNAL(userCanceled()), this, SLOT(removeNotificationUser()));
        connect(n, SIGNAL(actionTriggered(QString)),
                this, SLOT(notificationActionCalled(QString)));
        m_notifications[id] = n;
        m_layout->addWidget(n);
        n->show();
    }

    emit notificationAvailable();
    checkHeight();
}

void NotificationLayout::removeNotificationTimeout()
{
    Notification *n = qobject_cast<Notification*>(sender());
    if (!n)
    {
        qDebug() << "Oooook! TIMEOUT Expecting instance of notification, got:" << sender();
        return;
    }

    removeNotification(m_notifications.key(n), 1);
}

void NotificationLayout::removeNotificationUser()
{
    Notification *n = qobject_cast<Notification*>(sender());
    if (!n)
    {
        qDebug() << "Oooook! USERCANCEL Expecting instance of notification, got:" << sender();
        return;
    }

    removeNotification(m_notifications.key(n), 2);
}

void NotificationLayout::removeNotification(uint key, uint reason)
{
    Notification *n = m_notifications.take(key);
    if (!n)
    {
        qDebug() << "Oooook! Expecting instance of notification, got:" << key;
        return;
    }

    int ix = m_layout->indexOf(n);
    if (ix == -1)
    {
        qDebug() << "Qooook! Widget not in layout. Impossible!" << n;
        return;
    }

    delete m_layout->takeAt(ix);
    n->deleteLater();
    emit notificationClosed(key, reason);

    if (m_notifications.count() == 0)
        emit allNotificationsClosed();

    checkHeight();
}

void NotificationLayout::notificationActionCalled(const QString &actionKey)
{
    Notification *n = qobject_cast<Notification*>(sender());
    if (!n)
    {
        qDebug() << "Oooook! USERACTION Expecting instance of notification, got:" << sender();
        return;
    }

    emit actionInvoked(m_notifications.key(n), actionKey);
}

void NotificationLayout::checkHeight()
{
    int h = 0;
    QHashIterator<uint, Notification*> it(m_notifications);
    while (it.hasNext())
    {
        it.next();
        h +=it.value()->height() + m_layout->spacing();
    }

    setMinimumSize(width(), h);
    setMaximumSize(width(), h);

    emit heightChanged(h);
}

