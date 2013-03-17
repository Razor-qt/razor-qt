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


#include "razormountplugin.h"
#include "razormountconfiguration.h"
#include <razormount/razormount.h>
#include "actions/deviceaction.h"
#include "popup.h"
#include "mountbutton.h"

#include <QtDebug>

Q_EXPORT_PLUGIN2(mount, RazorMountPluginLibrary)


RazorMountPlugin::RazorMountPlugin(const IRazorPanelPluginStartupInfo &startupInfo):
    QObject(),
    IRazorPanelPlugin(startupInfo),
    mMountManager(new RazorMountManager(this)),
    mDeviceAction(0)
{
    mButton = new MountButton();
    mPopup = new Popup(mMountManager, this, mButton);

    settingsChanged();

    connect(mPopup, SIGNAL(visibilityChanged(bool)), mButton, SLOT(setDown(bool)));
    connect(mButton, SIGNAL(clicked()), mPopup, SLOT(showHide()));
    mMountManager->update();
}


RazorMountPlugin::~RazorMountPlugin()
{
    delete mButton;
    delete mDeviceAction;
}


QWidget *RazorMountPlugin::widget()
{
    return mButton;
}


void RazorMountPlugin::realign()
{
    mPopup->hide();
}


QDialog *RazorMountPlugin::configureDialog()
{
    mPopup->hide();
    RazorMountConfiguration *configWindow = new RazorMountConfiguration(*settings());
    configWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    return configWindow;
}


QIcon RazorMountPlugin::icon() const
{
    return mButton->icon();
}


void RazorMountPlugin::settingsChanged()
{
    QString s = settings()->value("newDeviceAction").toString();
    DeviceAction::ActionId actionId = DeviceAction::stringToActionId(s, DeviceAction::ActionMenu);

    delete mDeviceAction;
    mDeviceAction = DeviceAction::create(actionId, this);

    connect(mMountManager, SIGNAL(deviceAdded(RazorMountDevice*)),
             mDeviceAction, SLOT(deviceAdded(RazorMountDevice*)));

    connect(mMountManager, SIGNAL(deviceRemoved(RazorMountDevice*)),
             mDeviceAction, SLOT(deviceRemoved(RazorMountDevice*)));

}
