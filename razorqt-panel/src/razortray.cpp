#ifndef RAZORTRAY_CPP
#define RAZORTRAY_CPP
/**
 * @file razortray.cpp
 * @brief Implements Razortray and Razortraygui
 * @author Christopher "VdoP" Regali
 */

#include "razortray.h"
#include "razor.h"

/**
 * @brief this handles the events we get from the Razorplugin subsystem
 */
bool RazorTray::handleEvent(XEvent* _event)
{
    //we are waiting for a clientmessage
    if (_event->type == ClientMessage)
        if (_event->xclient.message_type == traycode )
        {
            qDebug() << "Razortray: got client Message for us";
            if (_event->xclient.data.l[1] == SYSTEM_TRAY_REQUEST_DOCK)
            {
                qDebug() << "Razortray: they wanna dock!" << _event->xclient.data.l[2];
                gui->swallowXEmbed(_event->xclient.data.l[2]);
            }
            return true;
        }
    return false;
}

/**
 * @brief constructor
 */
RazorTray::RazorTray(int _bar): RazorPlugin(_bar)
{
    qDebug() << "Razortray: initializing";
    //first we try to set "us" (meaning the razorbar we are in!) as selection owner for tray stuff, forcing the events to the bar-window
    //these will be sent to all plugins via the razorevent-subsystem
    Window barWinId=Razor::getInstance().get_gui()->getWindowList()->at(_bar);
    QString systrayselcode = "net_system_tray";
    QString managerselcode = "net_manager";
    Razor::getInstance().get_Xfitman()->setSelectionOwner(barWinId,systrayselcode, managerselcode);

    //get our traycode-atom
    traycode = Razor::getInstance().get_Xfitman()->getAtom("net_system_tray_opcode");

    //and dont forget to make the gui
    gui = new RazorTrayGUI(this);

    //now register us for the incoming events! (make razorevent delivarrrr)
    Razor::getInstance().get_events()->registerCallback(this);



    //now add us to the bar
    Razor::getInstance().get_gui()->addWidget(gui,_bar,0,Qt::AlignRight);
}


/**
 * @brief destructor
 */
RazorTray::~RazorTray()
{
    delete gui;
}

/**
 * @brief constructor
 */
RazorTrayGUI::RazorTrayGUI(RazorTray* _owner) : QLabel()
{
    // size of the tray is computed in updateLayout() to fit the
    // minimal required space (yes, QApplication::desktop()->width() causes
    // serious size problems in the multihead environments.
    //setFixedWidth(0.2*QApplication::desktop()->width());
    owner = _owner;
    layout = new QHBoxLayout;
    layout->setAlignment(Qt::AlignRight);
    layout->setContentsMargins(5, 0, 5, 0);
    layout->setSpacing(1);
    //setPixmap((QPixmap)(Razor::getInstance().get_looknfeel()->getPath() +  Razor::getInstance().get_looknfeel()->getValue("tray_bg")));
    setLayout(layout);
    show();
}

/**
 * @brief this makes the tray swallow an XEmbed into a new QX11EmbedContainer
 */
void RazorTrayGUI::swallowXEmbed(Window _wid)
{
    qDebug() << "Razortraygui: swallowing client: " << _wid;
    QX11EmbedContainer* embed = new QX11EmbedContainer(this);
    embed->embedClient(_wid);
    layout->addWidget(embed);
    embed->setContentsMargins(0, 0, 0, 0);
    embed->setFixedSize( height()-5,  height()-5);
    Razor::getInstance().get_Xfitman()->resizeWindow(_wid,embed->height(), embed->width());
    Razor::getInstance().get_Xfitman()->mapRaised(_wid);
    connect(embed, SIGNAL(clientClosed()), embed, SLOT(close()));
    qDebug()<<"DEBUG";
    updateLayout();
}

/**
 * @brief destructor
 */
RazorTrayGUI::~RazorTrayGUI()
{
    delete layout;
}

/**
 * @brief just updates the Layout
 */
void RazorTrayGUI::updateLayout()
{
    setLayout(layout);
    // grow the width as required
    setMinimumWidth(layout->count() * (height()-5));
}




#endif
