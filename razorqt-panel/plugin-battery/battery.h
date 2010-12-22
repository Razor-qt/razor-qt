#ifndef BATTERY_H
#define BATERRY_H

#include <QSystemDeviceInfo>
QTM_USE_NAMESPACE

#include "defs.h"
#include "razorbar.h"
#include "razorplugin.h"

/*! \brief Battery and power status display.

This plugin requires QtMonility library.

\author Petr Vanek <petr@scribus.info>
*/
class Battery : public RazorPluginSquare
{
    Q_OBJECT
public:
    Battery(RazorBar * panel, QWidget * parent, const QString & name);
    ~Battery() {};

private:
    QLabel * gui;
    QSystemDeviceInfo * info;

    void handleIcon();

private slots:
    void batteryLevelChanged(int level);
    void batteryStatusChanged(QSystemDeviceInfo::BatteryStatus status);
    void powerStateChanged(QSystemDeviceInfo::PowerState state);
};

extern "C" RazorPlugin* init(RazorBar* panel, QWidget* parent, const QString & name);

#endif
