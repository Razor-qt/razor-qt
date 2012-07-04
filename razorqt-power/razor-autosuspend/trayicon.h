/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Christian Surlykke <christian@surlykke.dk>
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSystemTrayIcon>
#include <QTime>
#include <QIcon>
#include <razorqt/razornotification.h>
#include "settings_dialog.h"
#include "razorqt/razorsettings.h"
#include "razorqt/razorpower/razorpower.h"
#include "lid.h"
#include "battery.h"


class TrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

public:
    explicit TrayIcon(QWidget *parent = 0);
    ~TrayIcon();

protected:
    void timerEvent(QTimerEvent *event);

public slots:
    void exitAutoSuspender();

private slots:
    void lidChanged(bool closed);
    void batteryChanged();
    void editSettings();
    void setupBatteryStatusIcons();

private:
    void doAction(int action);
    void makeContextMenu();
    int powerLowAction();

    void setStatusIcon();

    Lid lid;
    Battery battery;

    RazorPower razorPower;
    RazorSettings settings;
    GlobalRazorSettings globalSettings;

    RazorNotification razorNotification;
    QTime actionTime;

    bool m_hasBatteryStatusIcons;
    QIcon m_chargingBatteryStatusIcons[6]; // low, caution, 40%, 60%, 80%, 100%
    QIcon m_dechargingBatteryStatusIcons[6];

};

#endif // MAINWINDOW_H
