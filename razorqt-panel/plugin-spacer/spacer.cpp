#include "spacer.h"

RazorPlugin* init(RazorBar* panel, QWidget* parent, const QString & name)
{
    Spacer * ret = new Spacer(panel, parent, name);
    Q_ASSERT(ret);
    return ret;
}


Spacer::Spacer(RazorBar * panel, QWidget * parent, const QString & name)
    : RazorPlugin(panel, parent, name)
{
    gui = new QSpacerItem(1, 1,
                          panel->topbottom() ? QSizePolicy::Expanding : QSizePolicy::Minimum,
                          panel->topbottom() ? QSizePolicy::Minimum : QSizePolicy::Expanding
                        );
    mainLayout()->addItem(gui);
}

RazorPlugin::RazorPluginSizing Spacer::sizePriority()
{
    return RazorPlugin::Expanding;
}
