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


#ifndef RAZORMOUNT_H
#define RAZORMOUNT_H

#include "../panel/razorpanelplugin.h"
#include "../panel/razorpanel.h"

#include "mountbutton.h"

/*! 
\author Petr Vanek <petr@scribus.info>
*/
class RazorMount : public RazorPanelPlugin
{
    Q_OBJECT

public:
    RazorMount(const RazorPanelPluginStartInfo* startInfo, QWidget* parent = 0);
    ~RazorMount();

private:
    MountButton *m_button;

private slots:

};


EXPORT_RAZOR_PANEL_PLUGIN_H

#endif
