/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
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

