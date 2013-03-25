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
#include <QString>
#include <QDebug>

#include <razorqt/razorsettings.h>
#include "mainwindow.h"
#include "loginform.h"


#define CONFIG_FILE_PATH "/etc/lightdm/lightdm-razor-greeter.conf"
#define BACKGROUND_IMAGE_KEY "greeter-background-image"
#define LOGINFORM_OFFSETX_KEY "loginform-offset-x"
#define LOGINFORM_OFFSETY_KEY "loginform-offset-y"


MainWindow::MainWindow(int screen, QWidget *parent)
    : QWidget(parent),
        m_Screen(screen)
{
    setObjectName(QString("MainWindow_%1").arg(screen));
    
    
    QRect screenRect = QApplication::desktop()->screenGeometry(screen);
    setGeometry(screenRect);

    setBackgroundImage();    

    // display login dialog only in the main screen
    
    if (showLoginForm())
    {
        m_LoginForm = new LoginForm(this);
        
        int offsetX = getOffset(LOGINFORM_OFFSETX_KEY, screenRect.width() - m_LoginForm->width());
        int offsetY = getOffset(LOGINFORM_OFFSETY_KEY, screenRect.height() - m_LoginForm->height());
        
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

bool MainWindow::showLoginForm()
{
    return m_Screen == QApplication::desktop()->primaryScreen();
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

int MainWindow::getOffset(QString key, int maxVal)
{

    QSettings greeterSettings(CONFIG_FILE_PATH, QSettings::IniFormat);
    int offset = maxVal/2;
    if (greeterSettings.contains(key)) 
    {
        QString offsetAsString = greeterSettings.value(key).toString();
        if (QRegExp("^\\d+px$", Qt::CaseInsensitive).exactMatch(offsetAsString)) 
        {

            offset = offsetAsString.left(offsetAsString.size() - 2).toInt();
            if (offset > maxVal) offset = maxVal;
        }
        else if (QRegExp("^\\d+%$", Qt::CaseInsensitive).exactMatch(offsetAsString))
        {
            int offsetPct = offsetAsString.left(offsetAsString.size() -1).toInt();
            if (offsetPct > 100) offsetPct = 100;
            offset = (maxVal * offsetPct)/100;
        }
        else
        {
            qWarning() << "Could not understand" << key 
                       << "value:" << offsetAsString 
                       << "- must be of form <positivenumber>px or <positivenumber>%, e.g. 35px or 25%" ;
        }
    }

    return offset;
}

void MainWindow::setBackgroundImage()
{
    QImage backgroundImage;
    QSettings greeterSettings(CONFIG_FILE_PATH, QSettings::IniFormat);
    
    if (greeterSettings.contains(BACKGROUND_IMAGE_KEY))
    {
        QString pathToBackgroundImage = greeterSettings.value(BACKGROUND_IMAGE_KEY).toString();
        
        backgroundImage = QImage(pathToBackgroundImage);
        if (backgroundImage.isNull())
        {
            qWarning() << "Not able to read" << pathToBackgroundImage << "as image";
        }
             
    }
    
    if (backgroundImage.isNull())
    {
        backgroundImage = QImage(razorTheme.desktopBackground(m_Screen));
    }

    QPalette palette;
    QRect rect = QApplication::desktop()->screenGeometry(m_Screen);
    QBrush brush(backgroundImage.scaled(rect.width(), rect.height()));
    palette.setBrush(this->backgroundRole(), brush); 
    
    this->setPalette(palette);
}


