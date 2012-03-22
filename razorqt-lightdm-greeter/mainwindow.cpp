#include <QRect>
#include <QApplication>
#include <QDesktopWidget>
#include <QPalette>
#include "mainwindow.h"
#include "loginform.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    QRect screen = QApplication::desktop()->rect();
    setGeometry(screen);
    QImage image(QString(SHARE_DIR) + "/themes/light/simple_blue_widescreen.png");

    QPalette palette;
    palette.setBrush(this->backgroundRole(), QBrush(image.scaled(screen.width(), screen.right())));
    this->setPalette(palette);


    LoginForm *loginForm = new LoginForm(this);
    int offsetX = 2*screen.width()/5 - loginForm->width()/2;
    if (offsetX < 40)
    {
        offsetX = 40;
    }
    int offsetY = screen.height()/2 - loginForm->height()/2;
        loginForm->move(offsetX, offsetY);
    loginForm->show();
}

MainWindow::~MainWindow()
{
}
