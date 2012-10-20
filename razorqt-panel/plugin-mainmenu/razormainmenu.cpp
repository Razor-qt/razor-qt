/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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


#include "razormainmenu.h"
#include "razormainmenuconfiguration.h"
#include <QDebug>
#include <qtxdg/xdgdesktopfile.h>
#include <qtxdg/xmlhelper.h>
#include <QSettings>
#include <QFileInfo>
#include <QAction>
#include <QtCore/QTimer>
#include <QtGui/QMessageBox>
#include <razorqt/powermanager.h>
#include <razorqt/screensaver.h>
#include <razorqxt/qxtglobalshortcut.h>

#include <qtxdg/xdgicon.h>
#include <qtxdg/xdgdesktopfile.h>
#include <qtxdg/xdgmenuwidget.h>

#include <QPixmap>
#include <QStack>

#include <QCursor>
EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorMainMenu)


/************************************************

 ************************************************/
RazorMainMenu::RazorMainMenu(const RazorPanelPluginStartInfo* startInfo, QWidget* parent):
    RazorPanelPlugin(startInfo, parent),
    mMenu(0)
{
    setObjectName("MainMenu");

    layout()->setAlignment(Qt::AlignCenter);
    mButton.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mButton.setObjectName("Button");

    connect(&mButton, SIGNAL(clicked()), this, SLOT(showMenu()));
    connect(panel(), SIGNAL(panelRealigned()), this, SLOT(realign()));

    mPowerManager = new PowerManager(this);
    mPowerManager->setParentWidget(panel());

    mScreenSaver = new ScreenSaver(this);

    mShortcut = new QxtGlobalShortcut(this);
    connect(mShortcut, SIGNAL(activated()), this, SLOT(showHideMenu()));

    addWidget(&mButton);
    settingsChanged();
}


/************************************************

 ************************************************/
RazorMainMenu::~RazorMainMenu()
{
}


/************************************************

 ************************************************/
void RazorMainMenu::showHideMenu()
{
    if (mMenu && mMenu->isVisible())
        mMenu->hide();
    else
        showMenu();
}


/************************************************

 ************************************************/
void RazorMainMenu::showMenu()
{
    if (!mMenu)
        return;

    int x, y;

    switch (panel()->position())
    {
        case RazorPanel::PositionTop:
            x = mButton.mapToGlobal(QPoint(0, 0)).x();
            y = panel()->mapToGlobal(QPoint(0, panel()->height())).y();
            break;

        case RazorPanel::PositionBottom:
            x = mButton.mapToGlobal(QPoint(0, 0)).x();
            y = panel()->mapToGlobal(QPoint(0, 0)).y() - mMenu->sizeHint().height();
            break;

        case RazorPanel::PositionLeft:
            x = panel()->mapToGlobal(QPoint(panel()->width(), 0)).x();
            y = mButton.mapToGlobal(QPoint(0, 0)).y();
            break;

        case RazorPanel::PositionRight:
            x = panel()->mapToGlobal(QPoint(0, 0)).x() - mMenu->sizeHint().width();
            y = mButton.mapToGlobal(QPoint(0, 0)).y();
            break;

    }

    QPoint pos(x, y);
    mMenu->exec(pos);
}


/************************************************

 ************************************************/
void RazorMainMenu::settingsChanged()
{
    if (settings().value("showText", false).toBool())
    {
        mButton.setText(settings().value("text", "Start").toString());
        mButton.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
    else
    {
        mButton.setToolButtonStyle(Qt::ToolButtonIconOnly);
    }

    mLogDir = settings().value("log_dir", "").toString();

    QString mMenuFile = settings().value("menu_file", "").toString();
    if (mMenuFile.isEmpty())
        mMenuFile = XdgMenu::getMenuFileName();

    mXdgMenu.setEnvironments(QStringList() << "X-RAZOR" << "Razor");
    mXdgMenu.setLogDir(mLogDir);

    bool res = mXdgMenu.read(mMenuFile);
    connect(&mXdgMenu, SIGNAL(changed()), this, SLOT(buildMenu()));
    if (res)
    {
        QTimer::singleShot(1000, this, SLOT(buildMenu()));
    }
    else
    {
        QMessageBox::warning(this, "Parse error", mXdgMenu.errorString());
        return;
    }


    mShortcut->setShortcut(settings().value("shortcut", "ALT+F1").toString());
}


/************************************************

 ************************************************/
void RazorMainMenu::buildMenu()
{
    XdgMenuWidget *menu = new XdgMenuWidget(mXdgMenu, "", this);
    menu->setObjectName("TopLevelMainMenu");
    menu->setStyle(&mTopMenuStyle);

    menu->addSeparator();

    QMenu* leaveMenu = menu->addMenu(XdgIcon::fromTheme("system-shutdown"), tr("Leave"));
    leaveMenu->addActions(mPowerManager->availableActions());
    menu->addActions(mScreenSaver->availableActions());

    QMenu *oldMenu = mMenu;
    mMenu = menu;
    delete oldMenu;
}


/************************************************

 ************************************************/
void RazorMainMenu::showConfigureDialog()
{
    RazorMainMenuConfiguration *confWindow =
            this->findChild<RazorMainMenuConfiguration*>("MainMenuConfigurationWindow");

    if (!confWindow)
    {
        confWindow = new RazorMainMenuConfiguration(settings(), this);
    }

    confWindow->show();
    confWindow->raise();
    confWindow->activateWindow();
}

