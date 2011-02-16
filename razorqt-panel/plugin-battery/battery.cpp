#include "battery.h"

RazorPlugin* init(RazorBar* panel, QWidget* parent, const QString & name)
{
    Battery * ret = new Battery(panel, parent, name);
    Q_ASSERT(ret);
    return ret;
}


Battery::Battery(RazorBar * panel, QWidget * parent, const QString & name)
    : RazorPluginSquare(panel, parent, name)
{
    info = new QSystemDeviceInfo(this);
    gui = new QLabel(this);
    gui->setScaledContents(true);
    mainLayout()->addWidget(gui);

    connect(info, SIGNAL(batteryLevelChanged(int)),
            this, SLOT(batteryLevelChanged(int)));
    connect(info, SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)),
            this, SLOT(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)));
    connect(info, SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)),
            this, SLOT(powerStateChanged(QSystemDeviceInfo::PowerState)));

    handleIcon();
}

void Battery::batteryLevelChanged(int level)
{
    Q_UNUSED(level);
    handleIcon();
}

void Battery::batteryStatusChanged(QSystemDeviceInfo::BatteryStatus status)
{
    if (status == QSystemDeviceInfo::BatteryCritical)
    {
        QMessageBox::warning(this, tr("Battery Critical Level"),
                             tr("Battery level is less than 3%."));
    }
}

void Battery::powerStateChanged(QSystemDeviceInfo::PowerState state)
{
    Q_UNUSED(state);
    handleIcon();
}

void Battery::handleIcon()
{
    QString charging;
    QString fileLevel;
    int level = info->batteryLevel();

    switch (info->currentPowerState())
    {
        case QSystemDeviceInfo::UnknownPower:
            qDebug() << "Battery: UnknownPower";
        case QSystemDeviceInfo::WallPower:
            gui->setPixmap(QPixmap(":/battery-missing.png"));
            gui->setToolTip(tr("No battery found"));
            return; // exit here
            break;
        case QSystemDeviceInfo::WallPowerChargingBattery:
            charging = "-charging";
            break;
        case QSystemDeviceInfo::BatteryPower:
            break;
    }

    if (level <= 100 && level > 80)
        fileLevel = "100";
    else if (level <= 80 && level > 60)
        fileLevel = "080";
    else if (level <= 60 && level > 40)
        fileLevel = "060";
    else if (level <= 40 && level > 3)
        fileLevel = "040";
    else
        fileLevel = "low";

    QString icon(":/battery%1-%2");
    gui->setPixmap(icon.arg(charging, fileLevel));
    gui->setToolTip(tr("Charged: %1 %").arg(level));
}
