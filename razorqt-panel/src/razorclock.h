#ifndef RAZORCLOCK_H
#define RAZORCLOCK_H
#include "defs.h"
#include "razorplugin.h"


/**
 * @brief the clock-plugin for razorbar
 */
class RazorClock : public RazorPlugin
{
    Q_OBJECT
public:
    RazorClock(RazorBar * panel, QWidget * parent);
    ~RazorClock();

    int widthForHeight(int h)
    {
        gui->sizeHint().width();
    }
    int heightForWidth(int w)
    {
        return w;
    }
    RazorPlugin::RazorPluginSizing sizePriority()
    {
        return RazorPlugin::Static;
    }

public slots:
    void updateTime();

private:
    QTimer* clocktimer;
    QLabel * gui;
};

#endif
