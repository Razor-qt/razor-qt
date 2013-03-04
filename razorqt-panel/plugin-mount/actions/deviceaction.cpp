/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2013 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#include "deviceaction.h"
#include "deviceaction_info.h"
#include "deviceaction_menu.h"
#include "deviceaction_nothing.h"

#include "../menudiskitem.h"
#include "../razormountplugin.h"

#define ACT_NOTHING "nothing"
#define ACT_INFO    "showInfo"
#define ACT_MENU    "showMenu"

#define ACT_NOTHING_UPPER   QString(ACT_NOTHING).toUpper()
#define ACT_INFO_UPPER      QString(ACT_INFO).toUpper()
#define ACT_MENU_UPPER      QString(ACT_MENU).toUpper()

DeviceAction::DeviceAction(RazorMountPlugin *plugin, QObject *parent):
    mPlugin(plugin)
{
}

DeviceAction::~DeviceAction()
{
}

DeviceAction *DeviceAction::create(ActionId id, RazorMountPlugin *plugin, QObject *parent)
{
    switch (id)
    {
    case ActionNothing:
        return new DeviceActionNothing(plugin, parent);

    case ActionInfo:
        return new DeviceActionInfo(plugin, parent);

    case ActionMenu:
        return new DeviceActionMenu(plugin, parent);
    }

    return 0;
}

QString DeviceAction::actionIdToString(DeviceAction::ActionId id)
{
    switch (id)
    {
    case ActionNothing: return ACT_NOTHING;
    case ActionInfo:    return ACT_INFO;
    case ActionMenu:    return ACT_MENU;
    }

    return ACT_INFO;
}

void DeviceAction::deviceAdded(RazorMountDevice *device)
{
    if (MenuDiskItem::isUsableDevice(device))
        doDeviceAdded(device);
}

void DeviceAction::deviceRemoved(RazorMountDevice *device)
{
    if (MenuDiskItem::isUsableDevice(device))
        doDeviceRemoved(device);
}


DeviceAction::ActionId DeviceAction::stringToActionId(const QString &string, ActionId defaultValue)
{
    QString s = string.toUpper();
    if (s == ACT_NOTHING_UPPER) return ActionNothing;
    if (s == ACT_INFO_UPPER)    return ActionInfo;
    if (s == ACT_MENU_UPPER)    return ActionMenu;

    return defaultValue;
}

