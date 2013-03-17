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

/**
Razor panel plugins are standalone sharedlibraries
(*.so) located in PLUGIN_DIR (define provided by CMakeLists.txt).

Plugin for the panel is a library written on C++. One more necessary thing
is a .desktop file describing this plugin. The same may be additional files,
like translations. Themselves plugins will be installed to
/usr/local/lib/razor-panel or /usr/lib/razor-panel (dependent on cmake option
-DCMAKE_INSTALL_PREFIX). Desktop files are installed to
/usr/local/share/razor/razor-panel, translations to
/usr/local/share/razor/razor-panel/PLUGIN_NAME.
**/

class QDialog;

struct IRazorPanelPluginStartupInfo
{
    IRazorPanel *razorPanel;
    QSettings *settings;
    const RazorPluginInfo *desktopFile;
};


/** \brief Base abstract class for Razor panel widgets/plugins.
All plugins *must* be inherited from this one.

This class provides some basic API and inherited/implemented
plugins GUIs will be responsible on the functionality itself.

See <a href=https://github.com/Razor-qt/razor-qt/wiki/How-to-write-the-panel-plugin>
How to write the panel plugin</a> for more information about how to make your plugins.
**/

class IRazorPanelPlugin
{
public:
    /**
      This enum describes the properties of an plugin.
     **/
    enum Flag {
        NoFlags              = 0,   ///< It does not have any properties set.
        PreferRightAlignment = 1,   /**< The plugin is prefer right alignment (for example the clock plugin);
                                        otherwise plugin prefer left (like main menu).
                                        This flag is used only at the first start, later positions of all
                                        plugins saved in a config, and this saved information is used. */
        HaveConfigDialog     = 2    ///< The plugin have a configuration dialog.
    };

    Q_DECLARE_FLAGS(Flags, Flag)

    /**
      This enum describes the reason the plugin was activated.
     **/
    enum ActivationReason {
        Unknown             = 0,    ///< Unknown reason
        DoubleClick         = 2,    ///<	The plugin entry was double clicked
        Trigger             = 3,    ///<	The plugin was clicked
        MiddleClick         = 4     ///< The plugin was clicked with the middle mouse button
    };

    /**
     Constructs a IRazorPanelPlugin object with the given startupInfo. You do not have to worry
     about the startupInfo parameters, IRazorPanelPlugin process the parameters yourself.
     **/
    IRazorPanelPlugin(const IRazorPanelPluginStartupInfo &startupInfo):
        mSettings(startupInfo.settings),
        mPanel(startupInfo.razorPanel),
        mDesktopFile(startupInfo.desktopFile)
    {}

    /**
     Destroys the object.
     **/
    virtual ~IRazorPanelPlugin() {}

    /**
    Returns the plugin flags.
    The base class implementation returns a NoFlags.
     **/
    virtual Flags flags() const { return NoFlags; }

    /**
    Returns the string that is used in the theme QSS file.
    If you retuns "TeaTime" string, theme author may write something like `#TeaTime { border: 1px solid red; }`
    to set custom border for the your plugin.
    **/
    virtual QString themeId() const = 0;

    /**
     From users point of view plugin is a some visual widget on the panel. This function retuns pointer to it.
     This method called only once, so you are free to return pointer on class member, or create widget on the fly.
     **/
    virtual QWidget *widget() = 0;

    /**
    If you reimplement this function, you get direct access to all X events that the
    are received from the X server. The events are passed in the event parameter.

    The default implementation do nothing.
    **/
    virtual void x11EventFilter(XEvent *event) {}

    /**
    Returns the plugin settings dialog. Reimplement this function if your plugin has it.
    The panel does not take ownership of the dialog, it would probably a good idea to set Qt::WA_DeleteOnClose
    attribute for the dialog.
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

    /**
    This function is called when the user activates the plugin. reason specifies the reason for activation.
    IRazorPanelPlugin::ActivationReason enumerates the various reasons.

    The default implementation do nothing.
     **/
    virtual void activated(ActivationReason reason) {}

    /**
    This function is called when the panel geometry or lines count are changed.

    The default implementation do nothing.

     **/
    virtual void realign() {}

    /**
    Returns the panel object.
     **/
    IRazorPanel *panel() const { return mPanel; }


    QSettings *settings() const { return mSettings; }
    const RazorPluginInfo *desktopFile() const { return mDesktopFile; }

    /**
     Helper functions for calculating global screen position of some popup window with windowSize size.
     If you need to show some popup window, you can use it, to get global screen position for the new window.
     **/
    virtual QRect calculatePopupWindowPos(const QSize &windowSize)
    {
        return mPanel->calculatePopupWindowPos(this, windowSize);
    }


    virtual bool isSeparate() const { return false;  }
    virtual bool isExpandable() const { return false; }
private:
    QSettings *mSettings;
    IRazorPanel *mPanel;
    const RazorPluginInfo *mDesktopFile;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IRazorPanelPlugin::Flags)

/**
Every plugin must has the loader. You shoul only reimplement instance() method, and return your plugin.
Example:
@code
class RazorClockPluginLibrary: public QObject, public IRazorPanelPluginLibrary
{
    Q_OBJECT
    Q_INTERFACES(IRazorPanelPluginLibrary)
public:
    IRazorPanelPlugin *instance(const IRazorPanelPluginStartupInfo &startupInfo) { return new RazorClock(startupInfo);}
};
@endcode
**/
class IRazorPanelPluginLibrary
{
public:
    /**
     Destroys the IRazorPanelPluginLibrary object.
     **/
    virtual ~IRazorPanelPluginLibrary() {}

    /**
    Returns the root component object of the plugin. When the library is finally unloaded, the root component will automatically be deleted.
     **/
    virtual IRazorPanelPlugin* instance(const IRazorPanelPluginStartupInfo &startupInfo) = 0;
};


Q_DECLARE_INTERFACE(IRazorPanelPluginLibrary,
                    "razor-qt.org/Panel/PluginInterface/2.0")

#endif // IRAZORPANELPLUGIN_H
