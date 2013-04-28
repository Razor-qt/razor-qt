/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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


#include "razorpanelapplication.h"
#include "razorpanel.h"
#include <X11/Xlib.h>


/************************************************

 ************************************************/
RazorPanelApplication::RazorPanelApplication(int& argc, char** argv)
    : RazorApplication(argc, argv)
{
}

RazorPanelApplication::~RazorPanelApplication()
{
    qDeleteAll(mPanels);
}


/************************************************

 ************************************************/
void RazorPanelApplication::addPanel(const QString &cfg)
{
    RazorPanel *panel = new RazorPanel(cfg);
    mPanels << panel;
}


/************************************************

 ************************************************/
bool RazorPanelApplication::x11EventFilter(XEvent * event)
{
    foreach(RazorPanel *i, mPanels)
        i->x11EventFilter(event);
    return false;
}
