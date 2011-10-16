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

    virtual RazorPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog ; }

protected slots:
    virtual void showConfigureDialog();
    virtual void settigsChanged();

private:
    MountButton *m_button;

private slots:

};


EXPORT_RAZOR_PANEL_PLUGIN_H

#endif
