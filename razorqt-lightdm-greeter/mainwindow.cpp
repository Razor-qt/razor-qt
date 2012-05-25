/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Christian Surlykke
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include <QtCore/QRect>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QPalette>
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
