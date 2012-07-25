#include "batteryinfo.h"
#include "ui_batteryinfo.h"
#include <QStringList>
#include <QVariantMap>
#include <QTableWidgetItem>
#include <QDateTime>
#include <QDebug>

BatteryInfo::BatteryInfo(QVariantMap props, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BatteryInfo)
{
    ui->setupUi(this);

    qDebug() << "Ind i BatteryInfo, props:" << props;

    QDateTime UpdateTime;
    UpdateTime.setTime_t(props.value("UpdateTime").toULongLong());
    ui->updatedValue->setText(UpdateTime.toString("hh:mm:ss"));

    QString state = state2string(props.value("State", 0).toUInt());
    ui->stateValue->setText(state);

    QString energyFullDesign("%1 Wh");
    double efd = props.value("EnergyFullDesign").toDouble();
    ui->energyFullDesignValue->setText(energyFullDesign.arg(efd, 0, 'f', 2));

    QString energyFull("%1 Wh (%2 %)");
    double ef = props.value("EnergyFull").toDouble();
    double capacity = props.value("Capacity").toDouble();
    ui->energyFullValue->setText(energyFull.arg(ef, 0, 'f', 2).arg(capacity, 0, 'f', 1));

    QString energy("%1 Wh (%2 %)");
    double e = props.value("Energy").toDouble();
    double percentage = props.value("Percentage").toDouble();
    ui->energyValue->setText(energy.arg(e, 0, 'f', 2).arg(percentage, 0, 'f', 1));

    ui->energyRateValue->setText(QString::number(props.value("EnergyRate").toDouble(), 'f', 2) + " W");


    ui->modelValue->setText(props.value("Model").toString());

    int technology = props.value("Technology", 0).toInt();
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

    ui->voltageValue->setText(QString::number(props.value("Voltage").toDouble(), 'f', 2) + " V");

}

BatteryInfo::~BatteryInfo()
{
    delete ui;
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
