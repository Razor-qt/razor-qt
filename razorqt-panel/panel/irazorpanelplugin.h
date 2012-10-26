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


#ifndef IRAZORPANELPLUGIN_H
#define IRAZORPANELPLUGIN_H

#include <QtPlugin>
#include <QtGui/qwindowdefs.h> // For XEvent
#include <QSettings>
#include <razorqt/razorplugininfo.h>
#include "irazorpanel.h"

class QDialog;

struct IRazorPanelPluginStartupInfo
{
    IRazorPanel *razorPanel;
    QSettings *settings;
    const RazorPluginInfo *desktopFile;
};

class IRazorPanelPlugin
{
public:
    /**
      This enum describes the properties of an plugin.
     **/
    enum Flag {
        NoFlags              = 0,   //! It does not have any properties set.
        PreferRightAlignment = 1,   /*! The plugin is prefer right alignment (for example the clock plugin);
                                        otherwise plugin prefer left (like main menu).
                                        This flag is used only at the first start, later positions of all
                                        plugins saved in a config, and this saved information is used. */
        HaveConfigDialog     = 2    //! The plugin have a configuration dialog.
    };

    Q_DECLARE_FLAGS(Flags, Flag)

    IRazorPanelPlugin(const IRazorPanelPluginStartupInfo &startupInfo):
        mSettings(startupInfo.settings),
        mPanel(startupInfo.razorPanel),
        mDesktopFile(startupInfo.desktopFile)
    {}

    virtual ~IRazorPanelPlugin() {}

    /**
    Returns the plugin flags.
    The base class implementation returns a NoFlags.
     **/
    virtual Flags flags() const { return NoFlags; }

    /**
    Returns the plugin settings dialog. Reimplement this function if your plugin has it.
    **/
    virtual QString themeId() const = 0;

    virtual QWidget *widget() = 0;


    /**
    If you reimplement this function, you get direct access to all X events that the
    are received from the X server. The events are passed in the event parameter.

    The default implementation do nothing.
    **/
    virtual void x11EventFilter(XEvent *event) {}

    /**
    Returns the plugin settings dialog. Reimplement this function if your plugin has it.
    The panel takes ownership of the dialog; i.e., it will automatically delete.
    The default implementation returns 0, no dialog;

    Note that the flags method has to return HaveConfigDialog flag.
    To save the settings you should use a ready-to-use IRazorPanelPlugin::settings() object.

    **/
    virtual QDialog *configureDialog() { return 0; }

    /**
    This function is called when values are changed in the plugin settings.
    Reimplement this function to your plugin corresponded the new settings.

    The default implementation do nothing.
    **/
    virtual void settingsChanged() {}

    IRazorPanel *panel() const { return mPanel; }
    QSettings *settings() const { return mSettings; }
    const RazorPluginInfo *desktopFile() const { return mDesktopFile; }

private:
    QSettings *mSettings;
    IRazorPanel *mPanel;
    const RazorPluginInfo *mDesktopFile;
};



class IRazorPanelPluginLibrary
{
public:
    virtual ~IRazorPanelPluginLibrary() {}
    virtual IRazorPanelPlugin* instance(const IRazorPanelPluginStartupInfo &startupInfo) = 0;
};


Q_DECLARE_INTERFACE(IRazorPanelPluginLibrary,
                    "razor-qt.org/Panel/PluginInterface/2.0")

#endif // IRAZORPANELPLUGIN_H
