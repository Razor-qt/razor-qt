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

#include "notification.h"
#include "notificationarea.h"
#include <QtGui/QPainter>


Notification::Notification(const QString &application,
                           const QString &summary, const QString &body,
                           const QString &icon, int timeout,
                           QWidget *parent)
    : QWidget(parent),
      m_timer(0)
{
    setupUi(this);
    setObjectName("Notification");
    setMaximumWidth(NOTIFICATION_WIDTH);
    setMinimumWidth(NOTIFICATION_WIDTH);
    setMinimumHeight(100);

    setIcon(icon);
    setApplication(application);
    setSummary(summary);
    setBody(body);
    setTimeout(timeout);

    connect(closeButton, SIGNAL(clicked()), this, SLOT(closeButton_clicked()));
}

void Notification::setIcon(const QString &value)
{
    m_icon = QIcon::fromTheme(value);
    if (m_icon.isNull())
    {
        m_icon = QIcon(value); // Absolute path
    }

    iconLabel->setPixmap(m_icon.pixmap(64, 64));
}

void Notification::setApplication(const QString &value)
{
    m_application = value;
    appLabel->setText(value);
}

void Notification::setSummary(const QString &value)
{
    m_summary = value;
    summaryLabel->setText(value);
}

void Notification::setBody(const QString &value)
{
    m_body = value;
    bodyLabel->setText(value);
}

void Notification::setTimeout(int value)
{
    // Special values:
    //  < 0: server decides timeout
    //    0: infifite
    if (m_timer)
    {
        m_timer->stop();
        m_timer->deleteLater();
    }

    if (value != 0)
    {
        m_timer = new QTimer(this);
        connect(m_timer, SIGNAL(timeout()), this, SIGNAL(timeout()));

        if (value < 0)
        {
            value = 10000; // Default 10 secs. TODO: Get this from config file
        }

        m_timer->start(value);
    }
}

void Notification::closeButton_clicked()
{
    if (m_timer)
        m_timer->stop();
    emit userCanceled();
}

void Notification::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
