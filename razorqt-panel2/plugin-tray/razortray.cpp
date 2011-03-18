// Warning: order of those include is important.
#include <QApplication>
#include <QDesktopWidget>
#include "razortray.h"
#include <QtDebug>
#include <QX11EmbedContainer>
#include <QMessageBox>

#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrender.h>

#include "razorqt/xfitman.h"


EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorTray)


/**
 * @brief constructor
 */
RazorTray::RazorTray(RazorPanel* panel, const QString& configId, QWidget *parent)
    : RazorPanelPlugin(panel, configId, parent),
    m_count(0)
{
    qDebug() << "Razortray: initializing";
    setObjectName("Tray");
    setWindowTitle(tr("System Tray"));

    m_traycode = xfitMan().getAtom("net_system_tray_opcode");

    // DEBUG: do we have composite?
    qDebug() << "Composite:" << XGetSelectionOwner(QX11Info::display(), XInternAtom (QX11Info::display(), "_NET_WM_CM_S0", False));

    // Inform X that this is the tray
    Window winId = panel->winId();

    // Tray orientation
    int orientation = _NET_SYSTEM_TRAY_ORIENTATION_HORZ;
    XChangeProperty(QX11Info::display(),
                    winId,
                    XInternAtom(QX11Info::display(), "_NET_SYSTEM_TRAY_ORIENTATION", False),
                    XA_CARDINAL,
                    32,
                    PropModeReplace,
                    (unsigned char *) &orientation, // TODO/FIXME: vertical panels
                    1);
#if 0
    // This part of code is taken from tint2. But it looks like it's useles for now
    // as Qt4 does all for us. See Qt4 source - qwidget_x11.cpp and qx11embedcontainer_x11.cpp
    // So it seems to me that QToolBar is broken. See note in RazorTray::swallowXEmbed

    // X alpha background handling
    XVisualInfo templ;
    templ.screen = 0;
    templ.depth = 32;
    templ.c_class = TrueColor; // workaround for C++ - seet X11/Xutil.h

    int nvi;
    XVisualInfo *xvi = XGetVisualInfo(QX11Info::display(), VisualScreenMask|VisualDepthMask|VisualClassMask, &templ, &nvi);

    Visual *visual = 0;
    if (xvi) {
        qDebug() << "Found XVI" << xvi->screen << xvi->depth << xvi->c_class;
        int i;
        XRenderPictFormat *format;
        for (i = 0; i < nvi; i++) {
            format = XRenderFindVisualFormat(QX11Info::display(), xvi[i].visual);
            if (format->type == PictTypeDirect && format->direct.alphaMask) {
                visual = xvi[i].visual;
                qDebug() << "Found visual" << visual;
                break;
            }
        }
    }
    XFree (xvi);

    VisualID vid = XVisualIDFromVisual(visual);

    //VisualID vid =  XVisualIDFromVisual(DefaultVisual(QX11Info::display(), DefaultScreen(QX11Info::display())));
    XChangeProperty(QX11Info::display(),
                    winId,
                    XInternAtom(QX11Info::display(), "_NET_SYSTEM_TRAY_VISUAL", False),
                    XA_VISUALID,
                    32,
                    PropModeReplace,
                    (unsigned char*)&vid,
                    1);
#endif

    xfitMan().setSelectionOwner(winId, "net_system_tray", "net_manager");

    //setIconSize(QSize(32,32));
    //updateSize();

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
    embed->setContentsMargins(0, 0, 0, 0);
    embed->setFixedSize( 32, 32);

    qDebug() << embed->error();


    // WTF?!
    // If I try to call addWidget(embed) I get bunch of X errors:
    //    http://paste.opensuse.org/52197150
    // but if I call it as a normal widget (show()) it works.
    // It's the same as in old razor-panel - it was qwidget based...

    addWidget(embed);
    //embed->show();

    embed->embedClient(_wid);
    xfitMan().resizeWindow(_wid, embed->height(), embed->width());
    xfitMan().mapRaised(_wid);

    m_count++;
    //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //updateSize();

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
    m_count--;
    updateSize();
}

void RazorTray::updateSize()
{
    // TODO/FIXME: this does not work
    //int s = (m_count ? m_count : 1) * iconSize().width();
    //qDebug() << "RazorTray::updateSize" << m_count << s;
    //QWidget::resize(s, height());
    //QWidget::setMinimumWidth(s);
}
