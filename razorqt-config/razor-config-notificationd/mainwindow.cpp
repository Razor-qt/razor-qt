#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "razornotification.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->showNotificationButton, SIGNAL(pressed()), this, SLOT(showNotification()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showNotification()
{
    RazorNotification::notify("test app",0,"none","some summary","body",QVariantMap(),4);
}
