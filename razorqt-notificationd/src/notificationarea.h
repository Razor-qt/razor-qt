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

#ifndef NOTIFICATIONAREA_H
#define NOTIFICATIONAREA_H

#include <QtGui/QScrollArea>
#include "notificationlayout.h"


class RazorSettings;


class NotificationArea : public QScrollArea
{
    Q_OBJECT
public:

    explicit NotificationArea(QWidget *parent = 0);

    NotificationLayout* layout() { return m_layout; }

public slots:
    void applySettings();

private:
    NotificationLayout *m_layout;
    RazorSettings *m_settings;

    QString m_placement;
    int m_spacing;

private slots:
    void setHeight(int contentHeight);
};

#endif // NOTIFICATIONAREA_H
