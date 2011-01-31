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

private slots:
    //! run command in the action
    void execAction(QAction*);
};

extern "C" RazorPlugin* init(RazorBar* panel, QWidget* parent, const QString & name);

#endif
