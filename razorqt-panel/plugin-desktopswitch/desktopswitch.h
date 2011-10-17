/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
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


#ifndef DESKTOPSWITCH_H
#define DESKTOPSWITCH_H

#include "../panel/razorpanelplugin.h"

class QSignalMapper;
class QButtonGroup;


/**
 * @brief Desktop switcher. A very simple one...
 */
class DesktopSwitch : public RazorPanelPlugin
{
    Q_OBJECT
public:
    DesktopSwitch(const RazorPanelPluginStartInfo* startInfo, QWidget* parent = 0);
    ~DesktopSwitch();

    virtual void x11EventFilter(XEvent* event);
    
private:
    QButtonGroup * m_buttons;
    QSignalMapper* m_pSignalMapper;
    int m_desktopCount;
    QStringList m_desktopNames;

    void wheelEvent(QWheelEvent* e);
    void setup();

private slots:
    void setDesktop(int desktop);
};


EXPORT_RAZOR_PANEL_PLUGIN_H

#endif
