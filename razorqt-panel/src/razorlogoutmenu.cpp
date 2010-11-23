#ifndef RAZOR_LOGMENU_CPP
#define RAZOR_LOGMENU_CPP
#include "razorlogoutmenu.h"
#include "razor.h"

/**
 * @file razorlogoutmenu.cpp
 * @brief implements the classes Razorlogoutmenu and Razorlogoutmenugui
 * @author Christopher "VdoP" Regali
 */


/**
 * @brief the constructor
 */
RazorLogoutMenu::RazorLogoutMenu(RazorBar * panel, QWidget * parent) : RazorPluginSquare(panel, parent)
{
    //we only need to make the gui
    gui = new RazorLogoutMenuGUI(this);
    mainLayout()->addWidget(gui);
}

/**
 * @brief the destructor
 */
RazorLogoutMenu::~RazorLogoutMenu()
{
}




/**
 * @brief this creates our mainmenu and gets the xdg-menus for it
 */
void RazorLogoutMenuGUI::createMenus()
{
    //make the menu
    mainMenu = new QMenu;
    // add the basic actions (shutdown and stuff)

    // 1 create the actions
    shutdown = new QAction((QIcon)(Razor::getInstance().get_looknfeel()->getPath() + Razor::getInstance().get_looknfeel()->getString("razorlogmenu_shutdown")),tr("Shutdown"),this);
    reboot = new QAction((QIcon)(Razor::getInstance().get_looknfeel()->getPath() + Razor::getInstance().get_looknfeel()->getString("razorlogmenu_reboot")),tr("Reboot"),this);
    logout = new QAction((QIcon)(Razor::getInstance().get_looknfeel()->getPath() + Razor::getInstance().get_looknfeel()->getString("razorlogmenu_logout")),tr("Logout"),this);
    about = new QAction((QIcon)(Razor::getInstance().get_looknfeel()->getPath() + Razor::getInstance().get_looknfeel()->getString("razorlogmenu_about")),tr("about"),this);

    // 2 link with handler
    connect(shutdown, SIGNAL(triggered()), Razor::getInstance().get_handler(),SLOT(sys_shutdown()));
    connect(reboot, SIGNAL(triggered()), Razor::getInstance().get_handler(),SLOT(sys_reboot()));
    connect(logout, SIGNAL(triggered()), Razor::getInstance().get_handler(),SLOT(sys_logout()));
    connect(about, SIGNAL(triggered()), Razor::getInstance().get_handler(),SLOT(gui_showabout()));

    mainMenu->addAction(about);
    mainMenu->addAction(logout);
    mainMenu->addAction(reboot);
    mainMenu->addAction(shutdown);

}

/**
 * @brief this initializes us
 */
RazorLogoutMenuGUI::RazorLogoutMenuGUI(RazorLogoutMenu * parent)
{
    makeUp();
    createMenus();
}

/**
 * @brief cleans up our mess
 */
RazorLogoutMenuGUI::~RazorLogoutMenuGUI()
{
    delete mainMenu;
    delete logout;
    delete reboot;
    delete shutdown;
    delete about;
}


/**
 * @brief makes up the menu and style
 */

void RazorLogoutMenuGUI::makeUp()
{
    int barheight = Razor::getInstance().get_looknfeel()->getInt("razorbar_height");
    setFixedSize(barheight -3, barheight -3);
    icon = Razor::getInstance().get_looknfeel()->getPath() + Razor::getInstance().get_looknfeel()->getString("razorlogmenu_icon");
    //setPixmap( ((QPixmap)icon ).scaled(barheight - 5, barheight - 5));
    picon = ((QPixmap)icon ).scaled(barheight - 5, barheight - 5);
    acticon = Razor::getInstance().get_looknfeel()->getPath() + Razor::getInstance().get_looknfeel()->getString("razorlogmenu_acticon");
    //setPixmap( ((QPixmap)icon ).scaled(barheight - 5, barheight - 5));
    actpicon = ((QPixmap)acticon ).scaled(barheight-2, barheight-2);
    setPixmap(picon);
    show();
}

/**
 * @brief makes the menu popup
 */

void RazorLogoutMenuGUI::mousePressEvent(QMouseEvent* _event)
{
    if (_event->button() == Qt::LeftButton)
    {
        QPoint popuppos = mapToGlobal(QPoint(0,0));
        popuppos.setY(popuppos.y()-mainMenu->sizeHint().height()-5);
        mainMenu->popup(popuppos);
    }
}


/**
 * @brief makes the menu glow on mouseover
 */

void RazorLogoutMenuGUI::enterEvent(QEvent* _event)
{
    Q_UNUSED(_event);
    setPixmap(actpicon);

}

/**
 * @brief makes the menu glow on mouseover
 */

void RazorLogoutMenuGUI::leaveEvent(QEvent* _event)
{
    Q_UNUSED(_event);
    setPixmap(picon);
}



#endif
