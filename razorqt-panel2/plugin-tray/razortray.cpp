
#include "razortray.h"
#include <QtDebug>
#include <QX11EmbedContainer>
#include <X11/Xlib.h>
#include "razorqt/xfitman.h"


EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorTray)


/**
 * @brief constructor
 */
RazorTray::RazorTray(RazorPanel* panel, const QString& configId, QWidget *parent)
    : RazorPanelPlugin(panel, configId, parent)
{
    qDebug() << "Razortray: initializing";
    setObjectName("Tray");
    setWindowTitle(tr("System Tray"));
    //get our traycode-atom
    m_xfitman = new XfitMan();
    m_traycode = m_xfitman->getAtom("net_system_tray_opcode");

    //first we try to set "us" (meaning the razorbar we are in!) as selection owner
    //for tray stuff, forcing the events to the bar-window
    //these will be sent to all plugins via the razorevent-subsystem
    Window barWinId = panel->effectiveWinId();
    QString systrayselcode = "net_system_tray";
    QString managerselcode = "net_manager";
    qDebug() << "getSel" << m_xfitman->getSelectionOwner(systrayselcode) << barWinId;
    m_xfitman->setSelectionOwner(barWinId, systrayselcode, managerselcode);

    //now register us for the incoming events! (make razorevent delivarrrr)
    //Razor::getInstance().get_events()->registerCallback(this);
    connect(panel, SIGNAL(x11PropertyNotify(XEvent*)), this, SLOT(handleEvent(XEvent*)));
}


/**
 * @brief destructor
 */
RazorTray::~RazorTray()
{
//    delete gui;
}

/**
 * @brief this handles the events we get from the Razorplugin subsystem
 */
void RazorTray::handleEvent(XEvent* event)
{
    qDebug() << "RazorTray::handleEvent" << event->type << ClientMessage;
    //we are waiting for a clientmessage
    if (event->type == ClientMessage)
    {
        if (event->xclient.message_type == m_traycode )
        {
            qDebug() << "Razortray: got client Message for us";
            if (event->xclient.data.l[1] == SYSTEM_TRAY_REQUEST_DOCK)
            {
                qDebug() << "Razortray: they wanna dock!" << event->xclient.data.l[2];
                swallowXEmbed(event->xclient.data.l[2]);
            }
            //return true;
            return;
        }
    }
    //return false;
}

/**
 * @brief this makes the tray swallow an XEmbed into a new QX11EmbedContainer
 */
void RazorTray::swallowXEmbed(Window _wid)
{
    qDebug() << "Razortraygui: swallowing client: " << _wid;
    QX11EmbedContainer* embed = new QX11EmbedContainer(this);
    embed->embedClient(_wid);
    addWidget(embed);
    //embed->setContentsMargins(0, 0, 0, 0);
    //embed->setFixedSize( height()-5,  height()-5);
    //m_xfitman->resizeWindow(_wid,embed->height(), embed->width());
    m_xfitman->mapRaised(_wid);
    connect(embed, SIGNAL(clientClosed()), this, SLOT(closeEmbed()));
}

void RazorTray::closeEmbed()
{
    // it *must* be called by signal
    Q_ASSERT(sender());
    QX11EmbedContainer* embed = qobject_cast<QX11EmbedContainer*>(sender());
    Q_ASSERT(embed); // just to be sure
    embed->close();
    embed->deleteLater();
}
