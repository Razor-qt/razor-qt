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
#include <razorqt/razorconfigdialog.h>

#include "desktopconfigwindow.h"
#include "basicsettings.h"
#include "menuconfig.h"
#include "wmnativeconfig.h"

DesktopConfigWindow::DesktopConfigWindow(QWidget *parent) :
    RazorConfigDialog(tr("Razor Desktop Configuration"), new RazorSettings("desktop"), parent),
    mRestart(false)
{
    BasicSettings* basic = new BasicSettings(mSettings, this);
    addPage(basic, tr("Basic Settings"), "preferences-desktop");
    connect(basic, SIGNAL(needRestart()), this, SLOT(setRestart()));
    connect(this, SIGNAL(reset()), basic, SLOT(restoreSettings()));

    MenuConfig* menu = new MenuConfig(mSettings, this);
    addPage(menu, tr("Menu Configuration"), "show-menu");
    connect(menu, SIGNAL(needRestart()), this, SLOT(setRestart()));
    connect(this, SIGNAL(reset()), menu, SLOT(restoreSettings()));

    WMNativeConfig* wmNative = new WMNativeConfig(mSettings, this);
    addPage(wmNative, tr("WM Native Desktop"), "preferences-desktop-personal");
    connect(wmNative, SIGNAL(needRestart()), this, SLOT(setRestart()));
    connect(this, SIGNAL(reset()), wmNative, SLOT(restoreSettings()));

    connect(this, SIGNAL(reset()), SLOT(restoreSettings()));
}

DesktopConfigWindow::~DesktopConfigWindow()
{
    delete mSettings;
}

void DesktopConfigWindow::restoreSettings()
{
    mRestart = false;
}

void DesktopConfigWindow::closeEvent(QCloseEvent * event)
{
    Q_UNUSED(event);
    if (mRestart && QMessageBox::question(this, tr("Restart Desktop Module?"),
                            tr("Razor Desktop module needs to be restarted to apply changes. Restart?"),
                            QMessageBox::Yes, QMessageBox::No)
        == QMessageBox::Yes)
    {
        QProcess::execute("killall razor-desktop");
    }
}

void DesktopConfigWindow::setRestart()
{
    mRestart = true;
}
