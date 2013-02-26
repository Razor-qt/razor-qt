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
#include <QtGui/QX11Info>
#include <razorqt/razorsettings.h>

#include "mainwindow.h"
#include "loginform.h"


MainWindow::MainWindow(int screen, QWidget *parent)
    : QWidget(parent)
{
    setObjectName(QString("MainWindow_%1").arg(screen));

    QRect screenRect = QApplication::desktop()->screenGeometry(screen);
    setGeometry(screenRect);
    QImage image(razorTheme.desktopBackground(screen));

    QPalette palette;
    palette.setBrush(this->backgroundRole(), QBrush(image.scaled(screenRect.width(), screenRect.height())));
    this->setPalette(palette);

    // display login dialog only in the main screen
    m_main = screen == QApplication::desktop()->primaryScreen();
    if (m_main)
    {
        m_LoginForm = new LoginForm(this);
        int offsetX = 2*screenRect.width()/5 - m_LoginForm->width()/2;
        if (offsetX < 40)
        {
            offsetX = 40;
        }
        int offsetY = screenRect.height()/2 - m_LoginForm->height()/2;
        m_LoginForm->move(offsetX, offsetY);
        m_LoginForm->show();

        // This hack ensures that the primary screen will have focus
        // if there are more screens (move the mouse cursor in the center
        // of primary screen - not in the center of all X area). It
        // won't affect single-screen environments.
        int centerX = screenRect.width()/2 + screenRect.x();
        int centerY = screenRect.height()/2 + screenRect.y();
        QCursor::setPos(centerX, centerY);
    }
}

MainWindow::~MainWindow()
{
}

void MainWindow::setFocus(Qt::FocusReason reason)
{
    if (m_LoginForm)
    {
        m_LoginForm->setFocus(reason);
    }
    else 
    {
        QWidget::setFocus(reason);
    }
}