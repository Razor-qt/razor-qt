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


#ifndef SHOWDESKTOP_H
#define SHOWDESKTOP_H

#include "../panel/razorpanelplugin.h"
#include "../panel/razorpanel.h"


class QxtGlobalShortcut;


class ShowDesktop : public RazorPanelPlugin
{
    Q_OBJECT

public:
    ShowDesktop(const RazorPanelPluginStartInfo* startInfo, QWidget* parent = 0);

private:
    QxtGlobalShortcut * m_key;
    bool m_bInDesktopMode ; 

private slots:
    void showDesktop();
};

EXPORT_RAZOR_PANEL_PLUGIN_H

#endif

