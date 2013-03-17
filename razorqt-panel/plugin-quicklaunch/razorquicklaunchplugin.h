/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2013 Razor team
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


#ifndef RAZORQUICKLAUNCHPLUGIN_H
#define RAZORQUICKLAUNCHPLUGIN_H

#include "../panel/irazorpanelplugin.h"
#include <QObject>
class RazorQuickLaunch;

class RazorQuickLaunchPlugin: public QObject, public IRazorPanelPlugin
{
    Q_OBJECT
public:
    explicit RazorQuickLaunchPlugin(const IRazorPanelPluginStartupInfo &startupInfo);
    ~RazorQuickLaunchPlugin();

    virtual QWidget *widget();
    virtual QString themeId() const { return "QuickLaunch"; }

    void realign();

    bool isSeparate() const { return true; }

private:
    RazorQuickLaunch *mWidget;
};


class RazorQuickLaunchPluginLibrary: public QObject, public IRazorPanelPluginLibrary
{
    Q_OBJECT
    Q_INTERFACES(IRazorPanelPluginLibrary)
public:
    IRazorPanelPlugin *instance(const IRazorPanelPluginStartupInfo &startupInfo)
    {
        return new RazorQuickLaunchPlugin(startupInfo);
    }
};
#endif // RAZORQUICKLAUNCHPLUGIN_H
