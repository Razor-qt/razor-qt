// Warning: order of those include is important.
#include <QApplication>
#include <QDesktopWidget>
#include "razortray.h"
#include <QtDebug>
#include <QX11EmbedContainer>
#include <QMessageBox>

#include <QX11Info>
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

    m_traycode = xfitMan().getAtom("net_system_tray_opcode");
    // Inform X that this is the tray
    xfitMan().setSelectionOwner(winId(), "net_system_tray", "net_manager");
    
    connect(panel, SIGNAL(x11PropertyNotify(XEvent*)), this, SLOT(handleEvent(XEvent*)));
}


/**
 * @brief destructor
 */
RazorTray::~RazorTray()
{
}

/**
 * @brief this handles the events we get from the Razorplugin subsystem
 */
void RazorTray::handleEvent(XEvent* event)
{
    //qDebug() << "RazorTray::handleEvent" << event->type << ClientMessage;
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
                return;
            }
            else if (event->xclient.data.l[1] == SYSTEM_TRAY_BEGIN_MESSAGE)
            {
                qDebug() << "SYSTEM_TRAY_BEGIN_MESSAGE";
            }
            else if (event->xclient.data.l[1] == SYSTEM_TRAY_CANCEL_MESSAGE)
            {
                qDebug() << "SYSTEM_TRAY_CANCEL_MESSAGE";
            }
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
    qDebug() << "RazorTray::swallowXEmbed" << _wid;

    QX11EmbedContainer* embed = new QX11EmbedContainer();
    connect(embed, SIGNAL(error(QX11EmbedContainer::Error)),
            this, SLOT(embedError(QX11EmbedContainer::Error)));
//    connect(embed, SIGNAL(clientIsEmbedded()), this, SLOT(repaint()));

    embed->setObjectName("TrayObject");
    embed->embedClient(_wid);
    embed->setContentsMargins(0, 0, 0, 0);
    embed->setFixedSize( 32, 32);
    
    qDebug() << embed->error();

    xfitMan().resizeWindow(_wid, embed->height(), embed->width());
    xfitMan().mapRaised(_wid);
    
    //qDebug() << addWidget(embed) << embed->size();
    
    connect(embed, SIGNAL(clientClosed()), this, SLOT(closeEmbed()));
}

void RazorTray::embedError(QX11EmbedContainer::Error e)
{
    QMessageBox::warning(this, tr("Tray Error"),
                        tr("QX11EmbedContainer caused tray error: %1").arg(e));
}

void RazorTray::closeEmbed()
{
    qDebug() << "RazorTray::closeEmbed" << sender();
    // it *must* be called by signal
    Q_ASSERT(sender());
    QX11EmbedContainer* embed = qobject_cast<QX11EmbedContainer*>(sender());
    Q_ASSERT(embed); // just to be sure
    embed->close();
    embed->deleteLater();
}
