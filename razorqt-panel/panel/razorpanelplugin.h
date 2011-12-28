/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL3+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
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


/*! Razor panel plugins are standalone sharedlibraries
(*.so) located in PLUGIN_DIR (define provided by CMakeLists.txt).
Name of the plugin library is in this form:

    librazorpanel_<pluginname>.so

The <pluginname> variable is the same string as it's used
in session.conf file. Plugins are loaded on demand. See
RazorPluginManager::addPlugin().

Plugins like spinbutton, usually configured like spinbutton0,
spinbutton1, ..., spinButtonN does not contain numbers in
the library name. The config-number is provided to the plugin
as in the "name" constructor's argument.
*/


#ifndef RAZORPANELPLUGIN_H
#define RAZORPANELPLUGIN_H

#include <QtGui/QFrame>
#include <QtGui/QBoxLayout>
#include "razorpanel.h"

class QSettings;
class RazorSettings;
class QToolButton;
class QMenu;
class QStyleOptionToolBar;

/*! \brief Base abstract class for Razor panel widgets/plugins.
All plugins *must* be inherited from this one.

This class provides some basic API and inherited/implemented
plugins GUIs will be responsible on the functionality itself.

For better behavior handling there are arguments to provide
an access to Razor environment - see panel, parent.

For example, to correctly show the main menu, I need to know the
position of the panel (top, bottom, left, right) and panel size (height,
width).
*/

class RazorPanelPluginPrivate;
class RazorPluginInfo;

class RazorPanelPluginStartInfo
{
public:
    RazorPanelPluginStartInfo(const QSettings* _settings,
                              const QString& _configSection,
                              RazorPanel* _panel,
                              const RazorPluginInfo* _pluginInfo
                              ):
        settings(_settings),
        configSection(_configSection),
        panel(_panel),
        pluginInfo(_pluginInfo){}

    const QSettings* settings;
    const QString configSection;
    RazorPanel* panel;
    const RazorPluginInfo * pluginInfo;
};

class RazorPanelPlugin : public QFrame
{
    Q_OBJECT
    /*! This property holds whether the user can move the pluginr within the panel area.
        By default, this property is false.   */
    Q_PROPERTY(bool movable READ isMovable WRITE setMovable NOTIFY movableChanged)
    Q_PROPERTY(Alignment alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged)
    Q_ENUMS(Alignment)

public:
    enum Alignment {
        AlignLeft,
        AlignRight
    };

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

    /*! Standard plugin constructor.
    \param panel a reference to the RazorPanel, panel.
    \param configId defines section in a configuration file. Different instances of
           the plugin will keep the options in different sections of the config file.
    \param parent a reference to the QWidget parent. It might be a RazorPanel, but
                    it can be any QWidget.
    */
    explicit RazorPanelPlugin(const RazorPanelPluginStartInfo* startInfo, QWidget* parent = 0);
    virtual ~RazorPanelPlugin();

    /*! Returns the plugin flags.
        The base class implementation returns a NoFlags.
     */
    virtual Flags flags() const { return NoFlags; }


    RazorPanel* panel() const;
    QSettings& settings() const;
    QMenu* popupMenu() const;

    /*! If you reimplement this function, you get direct access to all X events that the
        are received from the X server. The events are passed in the event parameter.

        The default implementation do nothing.
    */
    virtual void x11EventFilter(XEvent* event);

    /*! Adds the given widget to the end of the plugin box.
        The plugin does not takes ownership of widget. */
    virtual void addWidget(QWidget* widget);

    bool isMovable() const;
    void setMovable(bool movable);

    Alignment alignment() const;
    void setAlignment(Alignment alignment);

    QString configId() const;

signals:
    void movableChanged(bool movable);
    void alignmentChanged();

public slots:
    void layoutDirectionChanged(QBoxLayout::Direction direction);
    /**
      Reimplement this function to show plugin settings dialog.
      To save the settings you should use a ready-to-use RazorPanelPlugin::settings() object.
     **/
    virtual void showConfigureDialog() {}

    /**
      This function can be reimplemented in order to modify or replace the plugin's context menu.
      The default menu is available through RazorPanelPlugin::popupMenu().
     **/
    virtual void showContextMenu(const QPoint &pos);

protected slots:
    /**
      This slot is called when values are changed in the plugin settings.
      Reimplement this function to your plugin corresponded the new settings.
    **/
    virtual void settigsChanged() {}

protected:
    void paintEvent(QPaintEvent* event);

    /*!
    If the value of the Qt property changes after the style sheet has been set, you will probably have to force a style sheet recomputation.
    This can be done by calling this method. */
    void updateStyleSheet();

private:
    RazorPanelPluginPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(RazorPanelPlugin)

    RazorPanelPlugin(const RazorPanelPlugin&);
    RazorPanelPlugin &operator=(const RazorPanelPlugin&);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(RazorPanelPlugin::Flags)


//! Prototype for plugin's init() function
typedef RazorPanelPlugin* (*PluginInitFunction)(const RazorPanelPluginStartInfo* startInfo, QWidget* parent);

/*! Helper macro for define RazorPanelPlugin.
    Place this macro in your plugin header file. */
#define EXPORT_RAZOR_PANEL_PLUGIN_H \
    extern "C" RazorPanelPlugin* init(const RazorPanelPluginStartInfo* startInfo, QWidget* parent);

/*! Helper macro for define RazorPanelPlugin.
    Place this macro in your plugin source file.  */
#define EXPORT_RAZOR_PANEL_PLUGIN_CPP(PLUGINCLASS)                      \
    RazorPanelPlugin* init(const RazorPanelPluginStartInfo* startInfo,  \
                           QWidget* parent)                             \
    {                                                                   \
        return new PLUGINCLASS(startInfo, parent);                      \
    }

#endif // RAZORPANELPLUGIN_H
