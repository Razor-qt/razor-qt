/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
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

#include <razorqt/razornotification.h>

#include "basicsettings.h"
#include "mainwindow.h"


BasicSettings::BasicSettings(RazorSettings* settings, QWidget *parent) :
    QWidget(parent),
    mSettings(settings)
{
    setupUi(this);

    restoreSettings();

    connect(topLeftButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(topRightButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(bottomRightButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(bottomLeftButton, SIGNAL(clicked()), this, SLOT(save()));

    connect(testButton, SIGNAL(clicked()), this, SLOT(testNotification()));

    RazorNotification serverTest;
    QString serverName = serverTest.serverInfo().name;
    if (serverName != "razor-notificationd")
    {
        if (serverName.isEmpty())
        {
            warningLabel->setText(tr(
                "<b>Warning:</b> No notifications daemon is running.\n"
                "A fallback will be used."));
        }
        else
        {
            warningLabel->setText(tr(
                "<b>Warning:</b> A third-party notifications daemon (%1) is running.\n"
                "These settings won't have an effect on it!"
                ).arg(serverName));
        }
    }
}

BasicSettings::~BasicSettings()
{
}

void BasicSettings::restoreSettings()
{
    QString placement = mSettings->value("placement", "bottom-right").toString().toLower();
    if (placement == "bottom-right")
        bottomRightButton->setChecked(true);
    else if (placement == "bottom-left")
        bottomLeftButton->setChecked(true);
    else if (placement == "top-right")
        topRightButton->setChecked(true);
    else if (placement == "top-left")
        topLeftButton->setChecked(true);
    else
        bottomRightButton->setChecked(true);
}

void BasicSettings::save()
{
    if (bottomRightButton->isChecked())
        mSettings->setValue("placement", "bottom-right");
    else if (bottomLeftButton->isChecked())
        mSettings->setValue("placement", "bottom-left");
    else if (topRightButton->isChecked())
        mSettings->setValue("placement", "top-right");
    else if (topLeftButton->isChecked())
        mSettings->setValue("placement", "top-left");
}

void BasicSettings::testNotification()
{
    RazorNotification::notify(//"razor-config-notificationd",
                              tr("Notification Summary"),
                              tr("Notification Body") + "\n\nLorem ipsum dolor sit amet...",
                              "razor-logo.png");
}
