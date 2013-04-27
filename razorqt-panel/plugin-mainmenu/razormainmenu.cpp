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
#include <razor-global-key-shortcuts-client/razor-global-key-shortcuts-client.h>
#include <razorqt/xfitman.h>

#include <qtxdg/xdgicon.h>
#include <qtxdg/xdgdesktopfile.h>
#include <qtxdg/xdgmenuwidget.h>

#include <QPixmap>
#include <QStack>

#include <QCursor>

Q_EXPORT_PLUGIN2(mainmenu, RazorMainMenuPluginLibrary)

#define DEFAULT_SHORTCUT "Alt+F1"

/************************************************

 ************************************************/
RazorMainMenu::RazorMainMenu(const IRazorPanelPluginStartupInfo &startupInfo):
    QObject(),
    IRazorPanelPlugin(startupInfo),
    mMenu(0),
    mShortcut(0),
    mLockCascadeChanges(false)
{
    mButton.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    connect(&mButton, SIGNAL(clicked()), this, SLOT(showMenu()));

    mPowerManager = new PowerManager(this);
    mScreenSaver = new ScreenSaver(this);

    settingsChanged();

    connect(mShortcut, SIGNAL(activated()), this, SLOT(showHideMenu()));
    connect(mShortcut, SIGNAL(shortcutChanged(QString,QString)), this, SLOT(shortcutChanged(QString,QString)));
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
void RazorMainMenu::shortcutChanged(const QString &/*oldShortcut*/, const QString &newShortcut)
{
    if (!newShortcut.isEmpty())
    {
        mLockCascadeChanges = true;

        settings()->setValue("dialog/shortcut", newShortcut);
        settings()->sync();

        mLockCascadeChanges = false;
    }
}


/************************************************

 ************************************************/
void RazorMainMenu::showMenu()
{
    if (!mMenu)
        return;

    int x=0, y=0;

    switch (panel()->position())
    {
        case IRazorPanel::PositionTop:
            x = mButton.mapToGlobal(QPoint(0, 0)).x();
            y = panel()->globalGometry().bottom();
            break;

        case IRazorPanel::PositionBottom:
            x = mButton.mapToGlobal(QPoint(0, 0)).x();
            y = panel()->globalGometry().top() - mMenu->sizeHint().height();
            break;

        case IRazorPanel::PositionLeft:
            x = panel()->globalGometry().right();
            y = mButton.mapToGlobal(QPoint(0, 0)).y();
            break;

        case IRazorPanel::PositionRight:
            x = panel()->globalGometry().left() - mMenu->sizeHint().width();
            y = mButton.mapToGlobal(QPoint(0, 0)).y();
            break;
    }

    // Just using Qt`s activateWindow() won't work on some WMs like Kwin.
    // There are two solutions:
    //  activate window with Qt call and then execute menu 1ms later using timer,
    //  or use native X11 API calls:
    xfitMan().raiseWindow(mButton.effectiveWinId());
    mMenu->exec(QPoint(x, y));
}


/************************************************

 ************************************************/
void RazorMainMenu::settingsChanged()
{
    if (mLockCascadeChanges)
        return;

    if (settings()->value("showText", false).toBool())
    {
        mButton.setText(settings()->value("text", "Start").toString());
        mButton.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
    else
    {
        mButton.setText("");
        mButton.setToolButtonStyle(Qt::ToolButtonIconOnly);
    }

    mLogDir = settings()->value("log_dir", "").toString();

    QString mMenuFile = settings()->value("menu_file", "").toString();
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
        QMessageBox::warning(0, "Parse error", mXdgMenu.errorString());
        return;
    }


    QString shortcut = settings()->value("shortcut", DEFAULT_SHORTCUT).toString();
    if (shortcut.isEmpty())
        shortcut = DEFAULT_SHORTCUT;

    if (!mShortcut)
        mShortcut = GlobalKeyShortcut::Client::instance()->addAction(shortcut, QString("/panel/%1/main_menu/%2/show_hide").arg(QFileInfo(settings()->fileName()).baseName()).arg(settings()->group()), tr("Show/hide main menu"), this);
    else if (mShortcut->shortcut() != shortcut)
    {
        mShortcut->changeShortcut(shortcut);
    }

    realign();
}


/************************************************

 ************************************************/
void RazorMainMenu::buildMenu()
{
    XdgMenuWidget *menu = new XdgMenuWidget(mXdgMenu, "", &mButton);
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
QDialog *RazorMainMenu::configureDialog()
{
    return new RazorMainMenuConfiguration(*settings(), DEFAULT_SHORTCUT);
}


/************************************************

 ************************************************/
void RazorMainMenu::realign()
{
    QSize minSize = QSize(0, 0);
    QSize maxSize = QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    if (panel()->isHorizontal())
    {
        minSize.setHeight(panel()->lineCount() * panel()->lineSize());
        maxSize.setHeight(minSize.height());

        minSize.setWidth(minSize.height());
    }
    else
    {
        minSize.setWidth(panel()->lineCount() * panel()->lineSize());
        minSize.setHeight(panel()->lineSize());
    }

    mButton.setMinimumSize(minSize);
    mButton.setMaximumSize(maxSize);
    mButton.updateGeometry();
}

#undef DEFAULT_SHORTCUT
