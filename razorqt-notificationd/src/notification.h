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

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtGui/QIcon>
#include <QtCore/QDateTime>
#include "ui_notification.h"


class NotificationActionsWidget;
class NotificationTimer;


/*! Implementation of one notification.
 *
 * Notification on-click behavior is defined in mouseReleaseEvent()
 */
class Notification : public QWidget, public Ui::Notification
{
    Q_OBJECT
public:
    /*! Construct a notification.
     * Parameters are described in \c Notifyd::Notify()
     */
    explicit Notification(const QString &application,
                          const QString &summary, const QString &body,
                          const QString &icon, int timeout,
                          const QStringList& actions, const QVariantMap& hints,
                          QWidget *parent = 0);

    /*! Set new values (update) for existing notification.
     * Parameters are described in \c Notifyd::Notify()
     */
    void setValues(const QString &application,
                   const QString &summary, const QString &body,
                   const QString &icon, int timeout,
                   const QStringList& actions, const QVariantMap& hints);

    QString application() const;
    QString summary() const;
    QString body() const;

signals:
    //! the server set timeout passed. Notification should close itself.
    void timeout();
    //! User clicked the "close" button
    void userCanceled();
    /*! User selected some of actions provided
     * \param actionKey an action key
     */
    void actionTriggered(const QString &actionKey);

protected:
    void enterEvent(QEvent * event);
    void leaveEvent(QEvent * event);
    /*! Define on-click behavior in the notification area.
        Currently it implements:
            - if there is one action or at least one default action, this
               default action is triggered on click.
               \see NotificationActionsWidget::hasDefaultAction()
               \see NotificationActionsWidget::defaultAction() 
            - it tries to find caller window by
                a) application name. \see XfitMan::getApplicationName()
                b) window title. \see XfitMan::getWindowTitle()
              if it can be found the window is raised and the notification is closed
            - leave notification as-is.
    */
    void mouseReleaseEvent(QMouseEvent * event);

private:
    NotificationTimer *m_timer;

    QPixmap m_pixmap;

    NotificationActionsWidget *m_actionWidget;

    // mandatory for stylesheets
    void paintEvent(QPaintEvent *);
    QPixmap getPixmapFromHint(const QVariant &argument) const;
    QPixmap getPixmapFromString(const QString &str) const;

private slots:
    void closeButton_clicked();
};


/*! A timer with pause/resume functionality
 *
 */
class NotificationTimer : public QTimer
{
    Q_OBJECT
public:
    NotificationTimer(QObject *parent=0);

public slots:
    void start(int msec);
    void pause();
    void resume();

private:
    QDateTime m_startTime;
    qint64 m_intervalMsec;
};

#endif // NOTIFICATION_H
