#ifndef BATTERYINFO_H
#define BATTERYINFO_H

#include <QDialog>
#include <QVariantMap>

namespace Ui {
class BatteryInfo;
}

class BatteryInfo : public QDialog
{
    Q_OBJECT
    
public:
    explicit BatteryInfo(QVariantMap props, QWidget *parent = 0);
    ~BatteryInfo();
    static QString state2string(uint state);

private:
    Ui::BatteryInfo *ui;
};

#endif // BATTERYINFO_H
