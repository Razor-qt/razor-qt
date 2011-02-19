/********************************************************************
  Copyright: 2010 Alexander Sokoloff <sokoloff.a@gmail.ru>

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

/*! Prototype for plugin's init() function
 */
//typedef RazorPlugin* (*PluginInitFunction)(const RazorBar* panel,
//                                           QWidget* parent,
//                                           const QString & name);


#ifndef RAZORPANELPLUGIN_H
#define RAZORPANELPLUGIN_H

#include <QToolBar>


/*! \brief Base abstract class for Razor panel widgets/plugins.
All plugins *must* be inherited from this one.

This class provides some basic API and inherited/implemented
plugins GUIs will be responsible on the functionality itself.

//Plugins will construct its content on this widget (painter
//or subwidgets in layout, doesn't matter).

//The panel will register plugins' sizeChanged() signal
//with one global-size-recomputing slot to update required changes.

//For better behavior handling there are arguments to provide
//an access to Razor environment - see panel, parent.

//For example, to correctly show the main menu, I need to know the
//position of the panel (top, bottom, left, right) and panel size (height,
//width). Parent not necessarily will be panel, say for quicklaunch
//buttons parent may be some subpanel etc.
*/

class RazorPanelPlugin : public QToolBar
{
    Q_OBJECT
public:
    enum Alignment {
        AlignLeft,
        AlignRight
    };

    /*! Standard plugin constructor.
    \param panel a reference to the RazorBar, panel.
    \param parent a reference to the QWidget parent. It might be a RazorBar, but
                    it can be any QWidget.
    \param name a plugin name as it's provided from config file. Including
                optional string suffixes ("quicklaunch0" for quicklaunch plug etc.)
    \param f unused for now.
    */
    explicit RazorPanelPlugin(QWidget *parent = 0);
    virtual ~RazorPanelPlugin();

    /*! Preferred alignment of the plug-in, at the left (for example the main menu
        plugin), or on the right (like clock). Default implementation return AlignLeft.

        The result of this function is used only at the first start, later positions
        of all plug-ins saved in a config file, and this saved information is used.
     */
    virtual Alignment preferredAlignment() const { return AlignLeft; }

signals:

public slots:

};

#endif // RAZORPANELPLUGIN_H
