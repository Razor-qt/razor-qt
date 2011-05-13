/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
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

#ifndef DESKTOPPLUGIN_H
#define DESKTOPPLUGIN_H

#include <razorqt/readsettings.h>


class DesktopPlugin
{
public:
    DesktopPlugin(const QString & configId, ReadSettings * config, ReadTheme * theme)
        : m_config(config),
          m_theme(theme)
    {
    };

    virtual QString info() { return QObject::tr("No info available"); };

protected:
    ReadSettings * m_config;
    ReadTheme * m_theme;
};


/*! Prototype for plugin's init() function
 */
typedef DesktopPlugin* (*PluginInitFunction)(const QString & configId, ReadSettings * config, ReadTheme * theme);


/*! Helper macro for define RazorPanelPlugin.
    Place this macro in your plugin header file.
 */
#define EXPORT_RAZOR_DESKTOP_PLUGIN_H \
    extern "C" DesktopPlugin* init(const QString & configId, ReadSettings * config, ReadTheme * theme);

/*! Helper macro for define RazorPanelPlugin.
    Place this macro in your plugin source file.
 */
#define EXPORT_RAZOR_DESKTOP_PLUGIN_CPP(PLUGINCLASS)        \
    DesktopPlugin* init(const QString & configId, ReadSettings * config, ReadTheme * theme) \
    {                                                       \
        return new PLUGINCLASS(configId, config, theme);      \
    }


#endif
