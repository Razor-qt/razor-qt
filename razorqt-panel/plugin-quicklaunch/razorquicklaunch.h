#ifndef RAZORQUICKLAUNCH_H
#define RAZORQUICKLAUNCH_H

#include "razorplugin.h"
#include <razorqt/readsettings.h>

class XdgDesktopFile;


/*! \brief Loader for "quick launcher" icons in the panel.
\author Petr Vanek <petr@scribus.info>
*/
class RazorQuickLaunch : public RazorPlugin
{
    Q_OBJECT

public:
    RazorQuickLaunch(RazorBar * panel, QWidget * parent, const QString & name);
    ~RazorQuickLaunch();

    int widthForHeight(int h)
    {
        return width();
    }
    int heightForWidth(int w)
    {
        return height();
    }
    RazorPlugin::RazorPluginSizing sizePriority()
    {
        return RazorPlugin::Static;
    }

private:
    ReadSettings * cfg;
    QString m_configId;

    QMap<QString,XdgDesktopFile*> m_xdgFiles;

private slots:
    //! run command in the action
    void execAction(QAction*);
};

/*! \brief Special action representation for RazorQuickLaunch plugin.
It supports XDG desktop files or "legacy" launching of specified apps.
All process management is handled internally.
\author Petr Vanek <petr@scribus.info>
*/
class RazorQuickLaunchAction : public QAction
{
    Q_OBJECT
    
public:
    /*! Constructor for "legacy" launchers.
        \warning The XDG way is preferred this is only for older or non-standard apps
        \param name a name to display in tooltip
        \param exec a executable with path
        \param icon a valid QIcon
     */
    RazorQuickLaunchAction(const QString & name,
                           const QString & exec,
                           const QIcon & icon,
                           QWidget * parent);
    /*! Constructor for XDG desktop handlers.
        \param desktop name of a XDG desktop file. With or without path.
     */
    RazorQuickLaunchAction(const QString & desktop, QWidget * parent);

    //! Returns true if the action is valid (contains all required properties).
    bool isValid() { return m_valid; }

public slots:
    void execAction();

private:
    enum ActionType { ActionLegacy, ActionXdg };
    ActionType m_type;
    QString m_data;
    bool m_valid;
};

extern "C" RazorPlugin* init(RazorBar* panel, QWidget* parent, const QString & name);

#endif
