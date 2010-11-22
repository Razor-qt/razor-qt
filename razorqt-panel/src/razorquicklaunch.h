#ifndef RAZORQUICKLAUNCH_H
#define RAZORQUICKLAUNCH_H

#include "razorplugin.h"
#include <razorqt/readsettings.h>


/*! \brief Loader for "quick launcher" icons in the panel.
\author Petr Vanek <petr@scribus.info>
*/
class RazorQuickLaunch : public RazorPlugin
{
    Q_OBJECT

public:
    RazorQuickLaunch(QString cmd, RazorBar * panel, QWidget * parent);
    ~RazorQuickLaunch();

    int widthForHeight(int h)
    {
        return width();
    }
    int heightForWidth(int w)
    {
        return w;
    }
    RazorPlugin::RazorPluginSizing sizePriority()
    {
        return RazorPlugin::Static;
    }

private:
    ReadSettings * settings;

    void setupGUI(int height);

private slots:
    //! run command in the action
    void execAction(QAction*);
};

#endif
