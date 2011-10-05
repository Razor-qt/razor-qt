/* BEGIN_COMMON_COPYRIGHT_HEADER
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
 * version 3 of the License, or (at your option) any later version.
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

#ifndef RAZORDESKMAN_H
#define RAZORDESKMAN_H

/**
 * @file razordeskman.h
 * @author Christopher "VdoP" Regali
 * @brief declares Razordeskman
*/

#include <QFileSystemWatcher>

#include "razordeskicon.h"
#include <razorqt/razorsettings.h>
#include <desktopplugin.h>

typedef QMap<QString,RazorDeskIconBase*> IconMap;
typedef QMapIterator<QString,RazorDeskIconBase*> IconMapIterator;

EXPORT_RAZOR_DESKTOP_PLUGIN_H

/**
 * @brief all the iconManagers are derived from this!
 */
class RazorDeskManager : public QObject, public DesktopPlugin
{
    Q_OBJECT
public:
    RazorDeskManager(const QString & configId, RazorSettings * config);
    ~RazorDeskManager();

    QString info();

public slots:
    void saveIconState();
    void updateIconList();

private:
    void restoreIconState();

    IconMap m_iconList;
    QFileSystemWatcher * m_fsw;

    RazorSettings *deskicons;

    DesktopPlugin::IconLaunchMode m_launchMode;
};



#endif

