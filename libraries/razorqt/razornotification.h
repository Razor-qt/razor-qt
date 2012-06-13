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

#include <QtCore/QStringList>

class RazorNotificationPrivate;

/**
 * \brief Libnotify-style desktop notifications
 *
 * Spec: http://developer.gnome.org/notification-spec
 */
class RazorNotification : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief RazorNotification is an object that represents a single notification.
     * \param summary Summary text briefly describing the notification (required by the spec)
     */
    RazorNotification(const QString& summary, QObject* parent = 0);
    ~RazorNotification();

    enum CloseReason
    {
        //! The notification expired.
        Expired     = 1,
        //! The notification was dismissed by the user.
        Dismissed   = 2,
        //! The notification was closed by a call to close().
        ForceClosed = 3,
        //! Undefined/reserved reasons.
        Unknown     = 4
    };

    enum Urgency
    {
        UrgencyLow      = 0,
        UrgencyNormal   = 1,
        UrgencyCritical = 2
    };

    /*!
     * \brief Set the summary text briefly describing the notification
     */
    void setSummary(const QString& summary);

    /*!
     * \brief Set the detailed body text
     */
    void setBody(const QString& body);

    /*!
     * \brief Set an icon to display
     * \param iconName  Name of the icon
     */
    void setIcon(const QString& iconName);

    /*!
     * \brief Set action buttons for the notification. Whenever an action is
     *        activated, the actionActivated() signal is emitted with the list
     *        index of the activated action.
     * \param actions       List of action button titles
     * \param defaultAction Index of the default action which gets activated
     *                      when the notification body is clicked
     * \sa actionActivated()
     */
    void setActions(const QStringList& actions, int defaultAction = -1);

    /*!
     * \brief Set the timeout for the notification
     * \param timeout Milliseconds for timeout, or zero to never time out.
     */
    void setTimeout(int timeout);

    /*!
     * \brief Set notification hint.
     * \note  For description of Hints, see http://developer.gnome.org/notification-spec/#hints
     * \note  For D-Bus-to-Qt mappings, see https://qt-project.org/doc/qdbustypesystem.html
     * \param hint  Hint name
     * \param value The hint data
     */
    void setHint(QString hint, QVariant value);

    /*!
     * \brief Set the "urgency" hint
     * \param urgency
     */
    void setUrgencyHint(Urgency urgency);

    /*!
     * \brief Remove all hints that were set
     * \sa setHint()
     */
    void clearHints();

    /*!
     * \brief Convenience function to create and display a notification for the most common
     *        cases. For anything more complex, create a RazorNotification object, set the
     *        desired properties and call update(). (That's what this does internally.)
     * \sa RazorNotification()
     */
    static void notify(const QString& summary,
                const QString& body = QString(),
                const QString& iconName = QString()
            );

public slots:
    /*!
     * \brief Display the notification or update it if it's already visible
     */
    void update();

    /*!
     * \brief Causes a notification to be forcefully closed and removed from the user's view.
     * It can be used, for example, in the event that what the notification pertains to
     * is no longer relevant, or to cancel a notification with no expiration time.
     */
    void close();

signals:
    /*!
     * \brief Emitted when the notification is closed
     * \param reason How notification was closed
     */
    void notificationClosed(CloseReason reason);

    /*!
     * \brief Emitted when an action button is activated.
     * \param actionNumber Index of the actions array for the activated button.
     * \sa setActions()
     */
    void actionActivated(int actionNumber);

private:
    Q_DECLARE_PRIVATE(RazorNotification)
    RazorNotificationPrivate* const d_ptr;
};

#endif // RAZORNOTIFICATION_H
