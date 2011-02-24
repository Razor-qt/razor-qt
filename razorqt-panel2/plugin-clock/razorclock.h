#ifndef RAZORCLOCK_H
#define RAZORCLOCK_H

#include "../panel/razorpanelplugin.h"


class ReadSettings;
class QLabel;

/**
 * @brief the clock-plugin for razorbar
 */
class RazorClock : public RazorPanelPlugin
{
    Q_OBJECT
public:
    RazorClock(RazorPanel* panel, const QString& configId, QWidget *parent = 0);
    ~RazorClock();

    virtual Alignment preferredAlignment() const { return AlignRight; }

public slots:
    void updateTime();

private:
    QTimer* clocktimer;
    QLabel * gui;
    ReadSettings * cfg;
    QString m_configId;
    QString clockFormat;

};


EXPORT_RAZOR_PANEL_PLUGIN_H

#endif
