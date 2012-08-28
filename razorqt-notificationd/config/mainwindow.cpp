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

#include <QtCore/QProcess>
#include <QtGui/QMessageBox>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <razorqt/razorconfigdialog.h>

#include "mainwindow.h"
#include "basicsettings.h"
#include "advancedsettings.h"


MainWindow::MainWindow(QWidget *parent) :
    RazorConfigDialog(tr("Razor Notifications Configuration"), new RazorSettings("notifications"), parent)
{
    BasicSettings* basic = new BasicSettings(mSettings, this);
    addPage(basic, tr("Basic Settings"), "preferences-desktop-notification");
    connect(this, SIGNAL(reset()), basic, SLOT(restoreSettings()));

    AdvancedSettings* menu = new AdvancedSettings(mSettings, this);
    addPage(menu, tr("Advanced Settings"), "preferences-desktop-notification-bell");
    connect(this, SIGNAL(reset()), menu, SLOT(restoreSettings()));
}

MainWindow::~MainWindow()
{
    delete mSettings;
}
