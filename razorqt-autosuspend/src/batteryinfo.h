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
    explicit BatteryInfo(QWidget *parent = 0);
    ~BatteryInfo();
    void updateInfo(QVariantMap batteryProperties);
    static QString state2string(uint state);

private:
    Ui::BatteryInfo *ui;
};

#endif // BATTERYINFO_H
