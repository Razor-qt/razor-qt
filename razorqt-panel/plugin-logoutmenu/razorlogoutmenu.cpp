#ifndef RAZOR_LOGMENU_CPP
#define RAZOR_LOGMENU_CPP
#include "razorlogoutmenu.h"
#include "razor.h"

/**
 * @file razorlogoutmenu.cpp
 * @brief implements the classes Razorlogoutmenu and Razorlogoutmenugui
 * @author Christopher "VdoP" Regali
 */

RazorPlugin* init(RazorBar* panel, QWidget* parent, const QString & name)
{
    RazorLogoutMenu * ret = new RazorLogoutMenu(panel, parent, name);
    Q_ASSERT(ret);
    return ret;
}

/**
 * @brief the constructor
 */
RazorLogoutMenu::RazorLogoutMenu(RazorBar * panel, QWidget * parent, const QString & name)
    : RazorPluginSquare(panel, parent, name)
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
    shutdown = new QAction(QIcon(Razor::getInstance().theme()->icon("shutdown")),
                           tr("Shutdown"), this);
    reboot = new QAction(QIcon(Razor::getInstance().theme()->icon("reboot")),
                         tr("Reboot"), this);
    logout = new QAction(QIcon(Razor::getInstance().theme()->icon("logout")),
                         tr("Logout"), this);
    about = new QAction(QIcon(Razor::getInstance().theme()->icon("about")),
                        tr("about"), this);

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
    : QLabel(parent)
{
    setScaledContents(true);
    QPixmap pm(Razor::getInstance().theme()->icon("shutdown"));
    //pm = pm.scaled(parent->height(), parent->height());
    setPixmap(pm);

    createMenus();
    show();
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
 * @brief makes the menu popup
 */

void RazorLogoutMenuGUI::mousePressEvent(QMouseEvent* _event)
{
    if (_event->button() == Qt::LeftButton)
    {
        //QPoint popuppos = mapToGlobal(QPoint(0,0));
        //popuppos.setY(popuppos.y()-mainMenu->sizeHint().height()-5);
        //mainMenu->popup(popuppos);
        mainMenu->popup(QCursor::pos());
    }
}


/**
 * @brief makes the menu glow on mouseover
 */

void RazorLogoutMenuGUI::enterEvent(QEvent* _event)
{
    Q_UNUSED(_event);
    //setPixmap(actpicon);

}

/**
 * @brief makes the menu glow on mouseover
 */

void RazorLogoutMenuGUI::leaveEvent(QEvent* _event)
{
    Q_UNUSED(_event);
    //setPixmap(picon);
}



#endif
