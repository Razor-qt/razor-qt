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

#include "ui_notification.h"


class Notification : public QWidget, public Ui::Notification
{
    Q_OBJECT
public:
    explicit Notification(const QString &application,
                          const QString &summary, const QString &body,
                          const QString &icon, int timeout,
                          const QStringList& actions, const QVariantMap& hints,
                          QWidget *parent = 0);

    void setValues(const QString &application,
                   const QString &summary, const QString &body,
                   const QString &icon, int timeout,
                   const QStringList& actions, const QVariantMap& hints);

signals:
    void timeout();
    void userCanceled();
    void actionTriggered(const QString &actionText);

public slots:

private:
    QTimer *m_timer;

    QString m_application;
    QString m_summary;
    QString m_body;
    QPixmap m_pixmap;
    QStringList m_actions;
    QVariantMap m_hints;

    // mandatory for stulesheets
    void paintEvent(QPaintEvent *);
    QPixmap getPixmapFromHint(const QVariant &argument) const;
    QPixmap getPixmapFromString(const QString &str) const;

private slots:
    void closeButton_clicked();
};

#endif // NOTIFICATION_H
