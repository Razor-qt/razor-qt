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


#ifndef RAZORCPULOADPLUGIN_H
#define RAZORCPULOADPLUGIN_H


#include "../panel/irazorpanelplugin.h"
#include <QObject>
class RazorCpuLoad;

class RazorCpuLoadPlugin: public QObject, public IRazorPanelPlugin
{
    Q_OBJECT
public:
    explicit RazorCpuLoadPlugin(const IRazorPanelPluginStartupInfo &startupInfo);
    ~RazorCpuLoadPlugin();

    virtual IRazorPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog; }
    virtual QWidget *widget();
    virtual QString themeId() const { return "CpuLoad"; }

    bool isSeparate() const { return true; }
    QDialog *configureDialog();

protected:
    virtual void settingsChanged();

private:
    RazorCpuLoad *mWidget;
};


class RazorCpuLoadPluginLibrary: public QObject, public IRazorPanelPluginLibrary
{
    Q_OBJECT
    Q_INTERFACES(IRazorPanelPluginLibrary)
public:
    IRazorPanelPlugin *instance(const IRazorPanelPluginStartupInfo &startupInfo)
    {
        return new RazorCpuLoadPlugin(startupInfo);
    }
};


#endif // RAZORCPULOADPLUGIN_H
