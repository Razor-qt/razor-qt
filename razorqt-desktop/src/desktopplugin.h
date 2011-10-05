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

#ifndef DESKTOPPLUGIN_H
#define DESKTOPPLUGIN_H

#include <razorqt/razorsettings.h>


class DesktopPlugin
{
public:
    enum IconLaunchMode {
        SingleClick,
        DoubleClick
    };

    DesktopPlugin(const QString & configId, RazorSettings * config)
        : m_config(config)
    {
    };

    virtual QString info() { return QObject::tr("No info available"); };
    
    static IconLaunchMode launchModeFromString(const QString & txt)
    {
        if (txt.toLower() == "doubleclick") return DoubleClick;
        return SingleClick;
    }
    
    static QString stringFromLaunchMode(IconLaunchMode mode)
    {
        switch (mode)
        {
            case (SingleClick) : return "singleclick";
            case (DoubleClick) : return "doubleclick";
        }
    }

protected:
    RazorSettings * m_config;
};


/*! Prototype for plugin's init() function
 */
typedef DesktopPlugin* (*PluginInitFunction)(const QString & configId, RazorSettings * config);


/*! Helper macro for define RazorPanelPlugin.
    Place this macro in your plugin header file.
 */
#define EXPORT_RAZOR_DESKTOP_PLUGIN_H \
    extern "C" DesktopPlugin* init(const QString & configId, RazorSettings * config);

/*! Helper macro for define RazorPanelPlugin.
    Place this macro in your plugin source file.
 */
#define EXPORT_RAZOR_DESKTOP_PLUGIN_CPP(PLUGINCLASS)        \
    DesktopPlugin* init(const QString & configId, RazorSettings * config) \
    {                                                       \
        return new PLUGINCLASS(configId, config);      \
    }


#endif
