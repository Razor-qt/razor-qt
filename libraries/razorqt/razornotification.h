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

#ifndef RAZORNOTIFICATION_H
#define RAZORNOTIFICATION_H

#include <QtCore/QObject>

class RazorNotificationPrivate;

class RazorNotification : public QObject
{
    Q_OBJECT
public:
    RazorNotification(QObject* parent = 0);
    ~RazorNotification();

    void notify(const QString& summary, const QString& body=QString(), const QString& iconName=QString());
    void close();

    enum CloseReason
    {
        Expired     = 1, // The notification expired.
        Dismissed   = 2, // The notification was dismissed by the user.
        ForceClosed = 3, // The notification was closed by a call to CloseNotification.
        Unknown     = 4  // Undefined/reserved reasons.
    };

signals:
    void notificationClosed(CloseReason reason);

private:
    Q_DECLARE_PRIVATE(RazorNotification)
    RazorNotificationPrivate* const d_ptr;
};

#endif // RAZORNOTIFICATION_H
