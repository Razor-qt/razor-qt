#ifndef RAZORPLUGIN_H
#define RAZORPLUGIN_H

#include "defs.h"

class RazorBar;
class RazorPlugin;


/**
 * @file razorplugin.h
 * @brief declares Razorplugin
 * @author Christopher "VdoP" Regali
 */

/*! Razor panel plugins are standalone sharedlibraries
(*.so) located in PLUGIN_DIR (define provided by CMakeLists.txt).
Name of the plugin library is in this form:

    librazorpanel_<pluginname>.so

The <pluginname> variable is the same string as it's used
in plugins.conf file. Plugins are loaded on demand. See
RazorPluginManager::addPlugin().

Plugins like spinbutton, usually configured like spinbutton0,
spinbutton1, ..., spinButtonN does not contain numbers in
the library name. The config-number is provided to the plugin
as in the "name" constructor's argument.
*/

/*! Prototype for plugin's init() function
 */
typedef RazorPlugin* (*PluginInitFunction)(const RazorBar* panel,
                                           QWidget* parent,
                                           const QString & name);

/*! \brief Base abstract class for Razor panel widgets/plugins.
All plugins *must* be inherited from this one.

The main issues I want to solve are:
- horizontal or vertical panel
- task panel should take all space available

All plugins will use one base class, the RazorPlugin.
This class provides some basic API and inherited/implemented
plugins GUIs will be responsible on the functionality itself.

Plugins will construct its content on this widget (painter
or subwidgets in layout, doesn't matter).

The panel will register plugins' sizeChanged() signal
with one global-size-recomputing slot to update required changes.

For better behavior handling there are arguments to provide
an access to Razor environment - see panel, parent.

For example, to correctly show the main menu, I need to know the
position of the panel (top, bottom, left, right) and panel size (height,
width). Parent not necessarily will be panel, say for quicklaunch
buttons parent may be some subpanel etc.

See also RazorPluginUISquare, a helper class to provide square,
non-resizing plugin base.
*/
class RazorPlugin : public QWidget
{
    Q_OBJECT

public:
    /*! Hold the information how the widget should behave
    	in the panel resizing procedure.
    	Static = this widget will use size as is provided with
    			  widthForHeight() or heightForWidth() methods
    	Expanding = it means that the widget can grow and occupy
    				all free space (task panel for example);
    */
    enum RazorPluginSizing
    {
        Static,
        Expanding
    };

    /*! Standard plugin constructor.
    \param panel a reference to the RazorBar, panel.
    \param parent a reference to the QWidget parent. It might be a RazorBar, but
                    it can be any QWidget.
    \param name a plugin name as it's provided from config file. Including
                optional string suffixes ("quicklaunch0" for quicklaunch plug etc.)
    \param f unused for now.
    */
    RazorPlugin(const RazorBar* panel,
                QWidget* parent=0,
                const QString & name = 0,
                Qt::WindowFlags f=0)
            : QWidget(parent), m_panel(panel)
    {
    }

    //! return the widget's width if is the panel horizontally layouted
    virtual int widthForHeight(int h) = 0;
    //! return the widget's height if is the panel vertically layouted
    virtual int heightForWidth(int w) = 0;
    //! return the resize behavior description
    virtual RazorPlugin::RazorPluginSizing sizePriority() = 0;

    /*! Base layout for plugin "background", content holder.
    This (optional) layout is the main layout with no margins, no padding etc.
    Custom widgets are just appended with addWidget(). Painting
    and displaying is usually done in the custom widget.
    */
    virtual QBoxLayout* mainLayout()
    {
        QHBoxLayout * layout = new QHBoxLayout();
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);
        setLayout(layout);
        return layout;
    }

    virtual bool handleEvent(XEvent* _event)
    {
        Q_UNUSED(_event);
        return false;
    }

    void setWidth(int w)
    {
        setMaximumWidth(w);
        setMinimumWidth(w);
    }
    void setHeight(int h)
    {
        setMaximumHeight(h);
        setMinimumHeight(h);
    }
    
    const RazorBar* panel()
    {
        return m_panel;
    }

signals:
    /*! Plugin has to emit this signal in any case when it
    	changes its size.
    */
    void sizeChanged();
    
private:
    const RazorBar* m_panel;
};

/*! A helper plugin base class to simplify implementation
of square, non-expandable plugin/widget.
*/
class RazorPluginSquare : public RazorPlugin
{
    Q_OBJECT

public:
    RazorPluginSquare(const RazorBar* panel,
                      QWidget* parent=0,
                      const QString & name = 0,
                      Qt::WindowFlags f=0)
            : RazorPlugin(panel, parent, name, f)
    {
    }

    int widthForHeight(int h)
    {
        return h;
    }
    int heightForWidth(int w)
    {
        return w;
    }
    RazorPlugin::RazorPluginSizing sizePriority()
    {
        return RazorPlugin::Static;
    }
};


#endif
