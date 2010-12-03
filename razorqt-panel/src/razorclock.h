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
    RazorClock(RazorBar * panel, QWidget * parent, const QString & name=0);
    ~RazorClock();

    int widthForHeight(int h)
    {
        return gui->sizeHint().width();
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

extern "C" RazorPlugin* init(RazorBar* panel, QWidget* parent, const QString & name);

#endif
