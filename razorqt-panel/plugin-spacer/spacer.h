#ifndef SPACER_H
#define SPACER_H

#include "defs.h"
#include "razorbar.h"
#include "razorplugin.h"

/*! \brief Nothing displaying widget. It just provides expanding
space for panel.
\author Petr Vanek <petr@scribus.info>
*/
class Spacer : public RazorPlugin
{
    Q_OBJECT
public:
    Spacer(RazorBar * panel, QWidget * parent, const QString & name);
    ~Spacer() {};

    int widthForHeight(int h) { return 1; };
    int heightForWidth(int w) { return 1; };
    RazorPlugin::RazorPluginSizing sizePriority();

private:
    QSpacerItem * gui;

};

extern "C" RazorPlugin* init(RazorBar* panel, QWidget* parent, const QString & name);

#endif
