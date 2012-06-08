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

#ifndef NotificationLayout_H
#define NotificationLayout_H

#include "notification.h"


class NotificationLayout : public QWidget
{
    Q_OBJECT
public:
    explicit NotificationLayout(QWidget *parent);

    void setSizes(int space, int width);

signals:
    void allNotificationsClosed();
    void notificationAvailable();
    void heightChanged(int);
    void notificationClosed(uint id, uint reason);
    void actionInvoked(uint id, const QString &actionText);

public slots:
    void addNotification(uint id, const QString &application,
                         const QString &summary, const QString &body,
                         const QString &icon, int timeout,
                         const QStringList& actions, const QVariantMap& hints);
    // reason:
    //1 - The notification expired.
    //2 - The notification was dismissed by the user.
    //3 - The notification was closed by a call to CloseNotification.
    //4 - Undefined/reserved reasons.
    void removeNotification(uint id, uint reason);

private:
    QHash<uint, Notification*> m_notifications;
    QVBoxLayout *m_layout;

    void checkHeight();

private slots:
    void removeNotificationTimeout();
    void removeNotificationUser();
    void notificationActionCalled(const QString &actionText);
};

#endif // NotificationLayout_H
