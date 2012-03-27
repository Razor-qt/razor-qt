#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "razornotification.h"
#include <QDebug>

namespace
{
    const QString g_scRazorSettingsName ="razorqt-notify";
}

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow),
    m_settings( g_scRazorSettingsName )
{
    ui->setupUi(this);
    connect(ui->showNotificationButton, SIGNAL(pressed()), this, SLOT(showNotification()));

    QPoint size = m_settings.value("notification_size").toPoint();
    QPoint pos = m_settings.value("notification_position").toPoint();

    ui->sizeXspinBox->setMaximum(1000);
    ui->sizeXspinBox->setMinimum(0);
    ui->sizeYspinBox->setMaximum(1000);
    ui->sizeYspinBox->setMinimum(0);

    ui->PositionXBox->setMaximum(1000);
    ui->PositionXBox->setMinimum(0);
    ui->PositionYBox->setMaximum(1000);
    ui->PositionYBox->setMinimum(0);

    ui->sizeXspinBox->setValue(size.x());
    ui->sizeYspinBox->setValue(size.y());

    ui->PositionXBox->setValue(pos.x());
    ui->PositionYBox->setValue(pos.y());


    ui->opacitySlider->setMinimum(0);
    ui->opacitySlider->setMaximum(100);

    qreal opacityValue = m_settings.value("notification_opacity").toReal();
    ui->opacitySlider->setValue(100* opacityValue);

    connect ( ui->PositionXBox, SIGNAL(valueChanged(int)) , this , SLOT(positionChanged()) );
    connect ( ui->PositionYBox, SIGNAL(valueChanged(int)) , this , SLOT(positionChanged()) );

    connect ( ui->sizeXspinBox, SIGNAL(valueChanged(int)) , this , SLOT(sizeChanged()) );
    connect ( ui->sizeYspinBox, SIGNAL(valueChanged(int)) , this , SLOT(sizeChanged()) );

    connect ( ui->opacitySlider, SIGNAL(valueChanged(int)) , this , SLOT(opacityChanged(int)) );



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showNotification()
{
    RazorNotification::notify("test app",0,"none","some summary","body",QVariantMap(),-1);
}

void MainWindow::positionChanged()
{
    QPoint newPos ;
    newPos.setX(ui->PositionXBox->value());
    newPos.setY(ui->PositionYBox->value());
    m_settings.setValue("notification_position", newPos);

    m_settings.sync();
}

void MainWindow::sizeChanged()
{
    QPoint newSize ;
    newSize.setX(ui->sizeXspinBox->value());
    newSize.setY(ui->sizeYspinBox->value());
    m_settings.setValue("notification_size", newSize);

    m_settings.sync();
}

void MainWindow::opacityChanged(int value)
{

    qreal newVal = static_cast<qreal>(value) / static_cast<qreal>(100);
    qDebug() << "Opacity changed" << value << " newVal=" << newVal;
    m_settings.setValue("notification_opacity", newVal);

    m_settings.sync();
}

