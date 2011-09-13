/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * END_COMMON_COPYRIGHT_HEADER */
/* Based on a "MountTray" project - modified for Razor needs
    http://hatred.homelinux.net

    @date   2010-11-11
    @brief  Main application class: integrate all components

    Copyright (C) 2010 by hatred <hatred@inbox.ru>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the version 2 of GNU General Public License as
    published by the Free Software Foundation.

    For more information see LICENSE and LICENSE.ru files
*/

#ifndef MOUNTBUTTON_H
#define MOUNTBUTTON_H

#include <QtGui/QToolButton>
#include <QtDBus/QtDBus>
#include <QtGui/QWidgetAction>
#include <QtGui/QMenu>
#include <QtCore/QHash>

#include "menudiskitem.h"
#include "razormount/diskmonitor.h"
#include "razormount/storagemanager.h"
#include "../panel/razorpanel.h"


class MountButton : public QToolButton
{
    Q_OBJECT
public:
    MountButton(QWidget * parent, RazorPanel * panel);
    ~MountButton();

private slots:
    void onDiskAdded(DiskInfo info);
    void onDiskRemoved(DiskInfo info);

    void onDbusDeviceChangesMessage(QDBusObjectPath device);

    void onMediaMount(const QString &device);
    void onMediaEject(const QString &device);

    void showMenu();

private:
    void initialScanDevices();
    void addMenuItem(const DiskInfo &info);
    void removeMenuItem(const QString &device);
    void updateMenuItem(const QString &device, const QString &name, bool is_mounted);

    void showMessage(const QString &text);
    void showError(const QString &text);

private:
    QMenu                          *m_menu;
    QHash<QString, QWidgetAction*>  m_menu_items;

    DiskMonitor    _dm;
    StorageManager _sm;

    RazorPanel *m_panel;
    bool mInitialized;

};

#endif
