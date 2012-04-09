#ifndef BATTERY_H
#define BATTERY_H

#include <QObject>
#include <QDBusInterface>

class Battery : public QObject
{
    Q_OBJECT
signals:
    void levelChanged(double newPercentage);

public:
    Battery();
    ~Battery();

    static const int POWER_LOW_LEVEL=15;
    double percentage();
    bool powerLow();

private slots:
    void uPowerBatteryChanged();

private:
    QDBusInterface *uPower;
    QDBusInterface *uPowerBatteryDevice;
    double m_percentage;
    bool m_powerLow;
};
#endif
