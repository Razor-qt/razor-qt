/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
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


#ifndef RAZORTASKBARPLUGIN_H
#define RAZORTASKBARPLUGIN_H

#include "../panel/irazorpanel.h"
#include "../panel/irazorpanelplugin.h"
#include "razortaskbar.h"
#include <QDebug>
class RazorTaskBar;

class RazorTaskBarPlugin : public QObject, public IRazorPanelPlugin
{
    Q_OBJECT
public:
    RazorTaskBarPlugin(const IRazorPanelPluginStartupInfo &startupInfo);
    ~RazorTaskBarPlugin();

    QString themeId() const { return "TaskBar"; }
    virtual IRazorPanelPlugin::Flags flags() const { return HaveConfigDialog ; }

    QWidget *widget() { return mTaskBar; }
    QDialog *configureDialog();

    void settingsChanged() { mTaskBar->settingsChanged(); }
    void x11EventFilter(XEvent *event) { mTaskBar->x11EventFilter(event); }
    void realign();

    bool isSeparate() const { return true; }
    bool isExpandable() const { return true; }
private:
    RazorTaskBar *mTaskBar;
};

class RazorTaskBarPluginLibrary: public QObject, public IRazorPanelPluginLibrary
{
    Q_OBJECT
    Q_INTERFACES(IRazorPanelPluginLibrary)
public:
    IRazorPanelPlugin *instance(const IRazorPanelPluginStartupInfo &startupInfo) { return new RazorTaskBarPlugin(startupInfo);}
};

#endif // RAZORTASKBARPLUGIN_H
