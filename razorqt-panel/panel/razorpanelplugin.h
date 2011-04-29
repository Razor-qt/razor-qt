/********************************************************************
  Copyright: 2010-2011 Alexander Sokoloff <sokoloff.a@gmail.ru>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*********************************************************************/

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
class QToolButton;
class QMenu;
class QStyleOptionToolBar;

/*! \brief Base abstract class for Razor panel widgets/plugins.
All plugins *must* be inherited from this one.

This class provides some basic API and inherited/implemented
plugins GUIs will be responsible on the functionality itself.

Buttons are added by adding actions, using addAction() or insertAction().
Groups of buttons can be separated using addSeparator() or insertSeparator().
If a toolbar button is not appropriate, a widget can be inserted instead
using addWidget() or insertWidget()

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
    RazorPanelPluginStartInfo(const QString& _configFile,
                              const QString& _configSection,
                              RazorPanel* _panel,
                              const RazorPluginInfo* _pluginInfo
                              ):
        configFile(_configFile),
        configSection(_configSection),
        panel(_panel),
        pluginInfo(_pluginInfo){}

    const QString configFile;
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

    /*! Standard plugin constructor.
    \param panel a reference to the RazorPanel, panel.
    \param configId defines section in a configuration file. Different instances of
           the plugin will keep the options in different sections of the config file.
    \param parent a reference to the QWidget parent. It might be a RazorPanel, but
                    it can be any QWidget.
    */
    //explicit RazorPanelPlugin(const QString& configFile, const QString& configSection, RazorPanel* panel);
    explicit RazorPanelPlugin(const RazorPanelPluginStartInfo* startInfo, QWidget* parent = 0);
    virtual ~RazorPanelPlugin();

    /*! Preferred alignment of the plug-in, at the left (for example the main menu
        plugin), or on the right (like clock). Default implementation return AlignLeft.

        The result of this function is used only at the first start, later positions
        of all plug-ins saved in a config file, and this saved information is used.
     */
    virtual Alignment preferredAlignment() const { return AlignLeft; }

    RazorPanel* panel() const;
    QSettings& settings() const;


    /*! Adds the given widget to the end of the plugin box.
        The plugin does not takes ownership of widget. */
    virtual void addWidget(QWidget* widget);

    //! Returns the layout manager that is installed on this plugin.
    QBoxLayout* layout() const;

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
