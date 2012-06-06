#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include "razornotification.h"
#include <QtCore/QDebug>
#include <QtGui/QColorDialog>
#include <QtGui/QDesktopWidget>

namespace
{
    const QString g_scRazorSettingsName ="razorqt-notify";

    const QString g_scNotificationSize = "notification_size";
    const QString g_scNotificationPosition = "notification_position";
    const QString g_scNotificationOpacity = "notification_opacity";
    const QString g_scNotificationBckColor = "notification_backgroundColor";
}

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow),
    m_settings( g_scRazorSettingsName ),
    m_notificationId(0)
{
    ui->setupUi(this);
    connect(ui->showNotificationButton, SIGNAL(pressed()), this, SLOT(showNotification()));

    QPoint size = m_settings.value(g_scNotificationSize).toPoint();
    QPoint pos = m_settings.value(g_scNotificationPosition).toPoint();

    QDesktopWidget* pDesktop = QApplication::desktop();

    qint32 maxX = 0 , maxY = 0 ;
    for ( int i=0;i< pDesktop->screenCount();++i)
    {
        maxX += pDesktop->screenGeometry(i).width();
        maxY = qMax(maxY, pDesktop->screenGeometry(i).height());
    }
    qDebug() << maxX ;
    qDebug() << maxY ;

    ui->sizeXspinBox->setMaximum(400);
    ui->sizeXspinBox->setMinimum(200);
    ui->sizeYspinBox->setMaximum(300);// maximum
    ui->sizeYspinBox->setMinimum(100); //minimum height value

    ui->PositionXBox->setMaximum(maxX);
    ui->PositionXBox->setMinimum(0);
    ui->PositionYBox->setMaximum(maxY);
    ui->PositionYBox->setMinimum(0);

    ui->sizeXspinBox->setValue(size.x());
    ui->sizeYspinBox->setValue(size.y());

    ui->PositionXBox->setValue(pos.x());
    ui->PositionYBox->setValue(pos.y());


    ui->opacitySlider->setMinimum(0);
    ui->opacitySlider->setMaximum(100);

    qreal opacityValue = m_settings.value(g_scNotificationOpacity).toReal();
    ui->opacitySlider->setValue(100* opacityValue);

    connect( ui->PositionXBox, SIGNAL(valueChanged(int)) , this , SLOT(positionChanged()) );
    connect( ui->PositionYBox, SIGNAL(valueChanged(int)) , this , SLOT(positionChanged()) );

    connect( ui->sizeXspinBox, SIGNAL(valueChanged(int)) , this , SLOT(sizeChanged()) );
    connect( ui->sizeYspinBox, SIGNAL(valueChanged(int)) , this , SLOT(sizeChanged()) );

    connect( ui->opacitySlider, SIGNAL(valueChanged(int)) , this , SLOT(opacityChanged(int)) );
    connect( ui->colorPickerButton, SIGNAL(pressed()), this, SLOT(colorPickerClicked()));
}

MainWindow::~MainWindow()
{
	/*
    RazorNotification::closeNotification(m_notificationId);
	*/
    delete ui;
}

void MainWindow::showNotification()
{
	/*
    m_notificationId = RazorNotification::notify(tr("A testing application"),
                                                 "razor-logo.png",
                                                 tr("Notification Summary"),
                                                 tr("Notification Body") + "<p>Lorem ipsim dolor sir amet...");
	*/
}

void MainWindow::positionChanged()
{
    QPoint newPos ;
    newPos.setX(ui->PositionXBox->value());
    newPos.setY(ui->PositionYBox->value());
    m_settings.setValue(g_scNotificationPosition, newPos);

    m_settings.sync();
}

void MainWindow::sizeChanged()
{
    QPoint newSize ;
    newSize.setX(ui->sizeXspinBox->value());
    newSize.setY(ui->sizeYspinBox->value());
    m_settings.setValue(g_scNotificationSize, newSize);

    m_settings.sync();
}

void MainWindow::opacityChanged(int value)
{
    qreal newVal = static_cast<qreal>(value) / static_cast<qreal>(100);
    qDebug() << "Opacity changed" << value << " newVal=" << newVal;
    m_settings.setValue(g_scNotificationOpacity, newVal);

    m_settings.sync();
}

void MainWindow::colorPickerClicked()
{
    QColor init = m_settings.value(g_scNotificationBckColor).value<QColor>();
    QColor c = QColorDialog::getColor(init,this);
    if ( c.isValid() )
    {
        m_settings.setValue(g_scNotificationBckColor, c);
    }
}

