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


#include "razormount.h"
#include "razormountconfiguration.h"

#include <QtDebug>

#define ACT_SHOW_MENU "showMenu"
#define ACT_SHOW_INFO "showInfo"
#define ACT_NOTHING   "nothing"

Q_EXPORT_PLUGIN2(volume, RazorMountPluginLibrary)


RazorMount::RazorMount(const IRazorPanelPluginStartupInfo &startupInfo):
    QObject(),
    IRazorPanelPlugin(startupInfo)
{
    mButton = new MountButton(this);
    settingsChanged();
}

RazorMount::~RazorMount()
{
    delete mButton;
}

QWidget *RazorMount::widget()
{
    return mButton;
}

void RazorMount::realign()
{
    mButton->realign();
}

QDialog *RazorMount::configureDialog()
{
    mButton->hidePopup();
    RazorMountConfiguration *configWindow = new RazorMountConfiguration(*settings());
    configWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    return configWindow;
}


void RazorMount::settingsChanged()
{
    QString s = settings()->value("newDeviceAction", MountButton::DevActionInfo).toString();

    if (s == ACT_SHOW_MENU)
        mButton->setDevAction(MountButton::DevActionMenu);
    else if(s == ACT_NOTHING)
        mButton->setDevAction(MountButton::DevActionNothing);
    else
        mButton->setDevAction(MountButton::DevActionInfo);
}
