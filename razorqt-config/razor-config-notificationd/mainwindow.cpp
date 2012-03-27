#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "razornotification.h"

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

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showNotification()
{
    RazorNotification::notify("test app",0,"none","some summary","body",QVariantMap(),-1);
}
