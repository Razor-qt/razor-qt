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
#include <razorqt/razorsettings.h>
#include <QtDebug>
#include <QUuid>
#include <X11/Xlib.h>


RazorPanelApplication::RazorPanelApplication(int& argc, char** argv)
    : RazorApplication(argc, argv)
{
    RazorSettings s("panel");
    QStringList panels = s.value("panels").toStringList();

    Q_FOREACH(QString i, panels)
    {
        addPanel(i);
    }
}

RazorPanelApplication::~RazorPanelApplication()
{
    qDeleteAll(mPanels);
}

void RazorPanelApplication::addNewPanel()
{
    QString name("panel_" + QUuid::createUuid().toString());
    addPanel(name);
    
    RazorSettings s("panel");
    QStringList panels = s.value("panels").toStringList();
    panels << name;
    s.setValue("panels", panels);
}

void RazorPanelApplication::addPanel(const QString &name)
{
    RazorPanel *panel = new RazorPanel(name);
    mPanels << panel;
    connect(panel, SIGNAL(deletedByUser(RazorPanel*)),
            this, SLOT(removePanel(RazorPanel*)));
}

bool RazorPanelApplication::x11EventFilter(XEvent * event)
{
    foreach(RazorPanel *i, mPanels)
        i->x11EventFilter(event);
    return false;
}

void RazorPanelApplication::removePanel(RazorPanel* panel)
{
    Q_ASSERT(mPanels.contains(panel));

    mPanels.removeAll(panel);
    
    RazorSettings s("panel");
    QStringList panels = s.value("panels").toStringList();
    panels.removeAll(panel->name());
    s.setValue("panels", panels);
    
    panel->deleteLater();
}
