#include "batteryinfo.h"
#include "ui_batteryinfo.h"
#include <QStringList>
#include <QVariantMap>
#include <QTableWidgetItem>
#include <QDateTime>
#include <QDebug>

BatteryInfo::BatteryInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BatteryInfo)
{
    ui->setupUi(this);
    updateInfo(QVariantMap());
}

BatteryInfo::~BatteryInfo()
{
    delete ui;
}

void BatteryInfo::updateInfo(QVariantMap batteryProperties)
{
    QDateTime UpdateTime;
    UpdateTime.setTime_t(batteryProperties.value("UpdateTime").toULongLong());
    ui->updatedValue->setText(UpdateTime.toString("hh:mm:ss"));

    QString state = state2string(batteryProperties.value("State", 0).toUInt());
    ui->stateValue->setText(state);

    QString energyFullDesign("%1 Wh");
    double efd = batteryProperties.value("EnergyFullDesign").toDouble();
    ui->energyFullDesignValue->setText(energyFullDesign.arg(efd, 0, 'f', 2));

    QString energyFull("%1 Wh (%2 %)");
    double ef = batteryProperties.value("EnergyFull").toDouble();
    double capacity = batteryProperties.value("Capacity").toDouble();
    ui->energyFullValue->setText(energyFull.arg(ef, 0, 'f', 2).arg(capacity, 0, 'f', 1));

    QString energy("%1 Wh (%2 %)");
    double e = batteryProperties.value("Energy").toDouble();
    double percentage = batteryProperties.value("Percentage").toDouble();
    ui->energyValue->setText(energy.arg(e, 0, 'f', 2).arg(percentage, 0, 'f', 1));

    ui->energyRateValue->setText(QString::number(batteryProperties.value("EnergyRate").toDouble(), 'f', 2) + " W");


    ui->modelValue->setText(batteryProperties.value("Model").toString());

    int technology = batteryProperties.value("Technology", 0).toInt();
    switch (technology)
    {
    case 1:  ui->technologyValue->setText(tr("Lithium ion")); break;
    case 2:  ui->technologyValue->setText(tr("Lithium polymer")); break;
    case 3:  ui->technologyValue->setText(tr("Lithium iron phosphate")); break;
    case 4:  ui->technologyValue->setText(tr("Lead acid")); break;
    case 5:  ui->technologyValue->setText(tr("Nickel cadmium")); break;
    case 6:  ui->technologyValue->setText(tr("Nickel metal hybride")); break;
    default: ui->technologyValue->setText(tr("Unknown")); break;
    }

    ui->voltageValue->setText(QString::number(batteryProperties.value("Voltage").toDouble(), 'f', 2) + " V");

}


QString BatteryInfo::state2string(uint state)
{
    switch (state)
    {
        case 1:  return tr("Charging");
        case 2:  return tr("Discharging");
        case 3:  return tr("Empty");
        case 4:  return tr("Fully charged");
        case 5:  return tr("Pending charge");
        case 6:  return tr("Pending discharge");
        default: return tr("Unknown");
    }
}
