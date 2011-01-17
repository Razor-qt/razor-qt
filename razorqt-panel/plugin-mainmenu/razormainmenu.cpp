/********************************************************************
  Copyright: 2010 Alexander Sokoloff <sokoloff.a@gmail.ru>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*********************************************************************/

#include "razormainmenu.h"
#include "razor.h"
#include <QDebug>
#include <razorqt/xdgdesktopfile.h>
#include <razorqt/xdgmenu.h>
#include <razorqt/domhelper.h>
#include <QSettings>
#include <QFileInfo>
#include <QAction>
#include <razorqt/readsettings.h>

#include <razorqt/xdgenv.h>
#include <razorbar.h>
#include <razorqt/xdgicon.h>
#include <razorqt/xdgdesktopfile.h>

#include <QStack>

/************************************************

 ************************************************/
RazorPlugin* init(RazorBar* panel, QWidget* parent, const QString & name)
{
    qDebug() << "MainMenu: INIT";
    RazorMainMenu * ret = new RazorMainMenu(panel, parent, name);
    Q_ASSERT(ret);
    return ret;
}


/************************************************

 ************************************************/
RazorMainMenu::RazorMainMenu(RazorBar* panel, QWidget* parent, const QString& name):
    RazorPlugin(panel, parent, name)
{
    qDebug() << "MainMenu: create";
    mMenu = 0;
    mPanel = panel;

    mainLayout()->addWidget(&mButton);
    connect(&mButton, SIGNAL(clicked()), this, SLOT(showMenu()));
    setObjectName("MainMenu");

    settingsChanged();

    QSizePolicy sp = mButton.sizePolicy();
    sp.setVerticalPolicy(QSizePolicy::Minimum);
    mButton.setSizePolicy(sp);

}


/************************************************

 ************************************************/
RazorMainMenu::~RazorMainMenu()
{
}


/************************************************

 ************************************************/
void RazorMainMenu::showMenu()
{
    if (!mMenu)
        buildMenu();

    int x, y;

    switch (mPanel->position())
    {
        case RazorBar::Top:
            x = mButton.mapToGlobal(QPoint(0, 0)).x();
            y = mPanel->mapToGlobal(QPoint(0, mPanel->sizeHint().height())).y();
            break;

        case RazorBar::Bottom:
            x = mButton.mapToGlobal(QPoint(0, 0)).x();
            y = mPanel->mapToGlobal(QPoint(0, 0)).y() - mMenu->sizeHint().height();
            break;

        case RazorBar::Left:
            x = mPanel->mapToGlobal(QPoint(mPanel->sizeHint().width(), 0)).x();
            y = mButton.mapToGlobal(QPoint(0, 0)).y();
            break;

        case RazorBar::Right:
            x = mPanel->mapToGlobal(QPoint(0, 0)).x() - mMenu->sizeHint().width();
            y = mButton.mapToGlobal(QPoint(0, 0)).y();
            break;

    }

    QPoint pos(x, y);
    //mMenu->popup(pos);
    mMenu->exec(pos);
}


/************************************************

 ************************************************/
void RazorMainMenu::settingsChanged()
{
    ReadSettings readSettings("mainmenu");
    QSettings *settings = readSettings.settings();

    mButton.setText(settings->value("Text", "").toString());
    mLogDir = settings->value("Log dir", "").toString();

    mMenuStyle.setIconSize(settings->value("Icon size", 16).toInt());
    mTopMenuStyle.setIconSize(settings->value("Top icon size", 16).toInt());


    mMenuFile = settings->value("Menu File", "").toString();
    if (mMenuFile.isEmpty())
        mMenuFile = XdgMenu::getMenuFileName();

}


/************************************************

 ************************************************/
void RazorMainMenu::buildMenu()
{
    XdgMenu xdgMenu(mMenuFile);
    xdgMenu.setLogDir(mLogDir);

    bool res = xdgMenu.read();
    if (!res)
    {
        QMessageBox::warning(this, "Parse error", xdgMenu.errorString());
        return;
    }

    mMenu = new QMenu(this);

    QDomElement rootElement = xdgMenu.xml().documentElement();
    buildMenuLevel(mMenu, rootElement);
    mMenu->setStyle(&mTopMenuStyle);
    setMenuIcons(mMenu);

    mMenu->addSeparator();

    QMenu* leaveMenu = mMenu->addMenu(XdgIcon::fromTheme("system-shutdown", 48), tr("Leave"));

    QAction* act;
    act = leaveMenu->addAction(XdgIcon::fromTheme("system-log-out", 48), tr("End session"));
    connect(act, SIGNAL(triggered()), Razor::getInstance().get_handler(), SLOT(sys_logout()));

    act = leaveMenu->addAction(XdgIcon::fromTheme("system-reboot", 48), tr("Restart computer"));
    connect(act, SIGNAL(triggered()), Razor::getInstance().get_handler(), SLOT(sys_reboot()));


    act = leaveMenu->addAction(XdgIcon::fromTheme("system-shutdown", 48), tr("Tur off computer"));
    connect(act, SIGNAL(triggered()), Razor::getInstance().get_handler(), SLOT(sys_shutdown()));
}


/************************************************

 ************************************************/
void RazorMainMenu::buildMenuLevel(QMenu* menu, const QDomElement& element)
{
    menu->setStyle(&mMenuStyle);

    if (!element.attribute("title").isEmpty())
        menu->setTitle(element.attribute("title"));
    else
        menu->setTitle(element.attribute("name"));

    menu->setToolTip(element.attribute("comment"));
    menu->setWindowIconText(element.attribute("icon"));
    connect(menu, SIGNAL(aboutToShow()), this, SLOT(menuAboutToShow()));


    DomElementIterator it(element, "");
    while(it.hasNext())
    {
        QDomElement e = it.next();

        // Build submenu ........................
        if (e.tagName() == "Menu")
        {
            QMenu* subMenu = new QMenu(menu);
            menu->addMenu(subMenu);
            buildMenuLevel(subMenu, e);
        }

        //Build application link ................
        else if (e.tagName() == "AppLink")
        {
            QAction* action = new QAction(menu);
            QString title;
            if (!e.attribute("title").isEmpty())
                title = e.attribute("title");
            else
                title = e.attribute("name");


            if (!e.attribute("genericName").isEmpty())
                title += QString(" (%1)").arg(e.attribute("genericName"));

            action->setText(title);

            action->setToolTip(e.attribute("comment"));
            action->setData(e.attribute("desktopFile"));
            action->setIconText(e.attribute("icon"));

            connect(action, SIGNAL(triggered()), this, SLOT(runConmmand()));

            menu->addAction(action);

        }
    }

}

/************************************************

 ************************************************/
void RazorMainMenu::runConmmand()
{
    QAction* action=qobject_cast<QAction*>(sender());
    if (!action || !action->data().isValid())
        return;

    //qDebug() << "RazorMainMenu::runConmmand: desktopfile=" << action->data().toString();
    XdgDesktopFile* desktopFile = XdgDesktopFileCache::getFile(action->data().toString());

    if (desktopFile->isValid())
        desktopFile->startDetached();
}


/************************************************

 ************************************************/
void RazorMainMenu::menuAboutToShow()
{
    QMenu* menu=qobject_cast<QMenu*>(sender());
    if (menu)
        setMenuIcons(menu);
}


/************************************************

 ************************************************/
void RazorMainMenu::setMenuIcons(QMenu* parent)
{

    QObjectList childs = parent->children();

    QObjectList::iterator i;
    for(i = childs.begin(); i != childs.end(); ++i)
    {
        QMenu* m=qobject_cast<QMenu*>(*i);
        if (m && !m->windowIconText().isEmpty() && m->icon().isNull())
            m->setIcon(XdgIcon::fromTheme(m->windowIconText(), 48, parent->icon())) ;

        QAction* a=qobject_cast<QAction*>(*i);
        if (a && !a->iconText().isEmpty() && a->icon().isNull())
        {
            a->setIcon(XdgIcon::fromTheme(QStringList()
                                          << a->iconText()
                                          << "application-x-executable",
                                          48
                                          )
                       );
        }
     }
}

