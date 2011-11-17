/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
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


// Warning: order of those include is important.
#include <QDebug>
#include <QApplication>
#include <QtGui/QResizeEvent>
#include <QtGui/QPainter>
#include <QtGui/QBitmap>
#include <QtGui/QStyle>

#include "../panel/razorpanel.h"
#include "trayicon.h"
#include <QtGui/QX11Info>
#include "razorqt/xfitman.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xrender.h>

#define XEMBED_EMBEDDED_NOTIFY 0


static bool xError;


/************************************************

************************************************/
int windowErrorHandler(Display *d, XErrorEvent *e)
{
    d=d;e=e;
    xError = true;
    if (e->error_code != BadWindow) {
        char str[1024];
        XGetErrorText(d, e->error_code,  str, 1024);
        qWarning() << "Error handler" << e->error_code
                   << str;
    }
    return 0;
}


/************************************************

 ************************************************/
TrayIcon::TrayIcon(Window iconId, QWidget* parent):
    QFrame(parent),
    mIconId(iconId),
    mWindowId(0),
    mIconSize(TRAY_ICON_SIZE_DEFAULT, TRAY_ICON_SIZE_DEFAULT),
    mDamage(0)
{
    setObjectName("TrayIcon");

    mValid = init();
}



/************************************************

 ************************************************/
bool TrayIcon::init()
{
    Display* dsp = QX11Info::display();

    XWindowAttributes attr;
    if (! XGetWindowAttributes(dsp, mIconId, &attr)) return false;

//    qDebug() << "New tray icon ***********************************";
//    qDebug() << "  * window id:  " << hex << mIconId;
//    qDebug() << "  * window name:" << xfitMan().getName(mIconId);
//    qDebug() << "  * size (WxH): " << attr.width << "x" << attr.height;
//    qDebug() << "  * color depth:" << attr.depth;

    unsigned long mask = 0;
    XSetWindowAttributes set_attr;

    Visual* visual = attr.visual;
    set_attr.colormap = attr.colormap;
    set_attr.background_pixel = 0;
    set_attr.border_pixel = 0;
    mask = CWColormap|CWBackPixel|CWBorderPixel;

    mWindowId = XCreateWindow(dsp, this->winId(), 0, 0, mIconSize.width(), mIconSize.height(),
                              0, attr.depth, InputOutput, visual, mask, &set_attr);


    xError = false;
    XErrorHandler old;
    old = XSetErrorHandler(windowErrorHandler);
    XReparentWindow(dsp, mIconId, mWindowId, 0, 0);
    XSync(dsp, false);
    XSetErrorHandler(old);

    if (xError) {
        qWarning() << "****************************************";
        qWarning() << "* Not icon_swallow                     *";
        qWarning() << "****************************************";
        XDestroyWindow(dsp, mWindowId);
        return false;
    }


    {
        Atom acttype;
        int actfmt;
        unsigned long nbitem, bytes;
        unsigned char *data = 0;
        int ret;

        ret = XGetWindowProperty(dsp, mIconId, xfitMan().atom("_XEMBED_INFO"),
                                 0, 2, false, xfitMan().atom("_XEMBED_INFO"),
                                 &acttype, &actfmt, &nbitem, &bytes, &data);
        if (ret == Success) {
            if (data)
                XFree(data);
        }
        else {
            qWarning() << "TrayIcon: xembed error";
            XDestroyWindow(dsp, mWindowId);
            return false;
        }
    }

    {
        XEvent e;
        e.xclient.type = ClientMessage;
        e.xclient.serial = 0;
        e.xclient.send_event = True;
        e.xclient.message_type = xfitMan().atom("_XEMBED");
        e.xclient.window = mIconId;
        e.xclient.format = 32;
        e.xclient.data.l[0] = CurrentTime;
        e.xclient.data.l[1] = XEMBED_EMBEDDED_NOTIFY;
        e.xclient.data.l[2] = 0;
        e.xclient.data.l[3] = mWindowId;
        e.xclient.data.l[4] = 0;
        XSendEvent(dsp, mIconId, false, 0xFFFFFF, &e);
    }

    XSelectInput(dsp, mIconId, StructureNotifyMask);
    mDamage = XDamageCreate(dsp, mIconId, XDamageReportRawRectangles);
    XCompositeRedirectWindow(dsp, mWindowId, CompositeRedirectManual);

    XMapWindow(dsp, mIconId);
    XMapRaised(dsp, mWindowId);

    xfitMan().resizeWindow(mWindowId, mIconSize.width(), mIconSize.height());
    xfitMan().resizeWindow(mIconId,   mIconSize.width(), mIconSize.height());

    return true;
}


/************************************************

 ************************************************/
TrayIcon::~TrayIcon()
{
    Display* dsp = QX11Info::display();
    XSelectInput(dsp, mIconId, NoEventMask);

    if (mDamage)
        XDamageDestroy(dsp, mDamage);

    // reparent to root
    xError = false;
    XErrorHandler old = XSetErrorHandler(windowErrorHandler);

    XUnmapWindow(dsp, mIconId);
    XReparentWindow(dsp, mIconId, QX11Info::appRootWindow(), 0, 0);

    XDestroyWindow(dsp, mWindowId);
    XSync(dsp, False);
    XSetErrorHandler(old);
}


/************************************************

 ************************************************/
QSize TrayIcon::sizeHint() const
{
    QMargins margins = contentsMargins();
    return QSize(margins.left() + mIconSize.width() + margins.right(),
                 margins.top() + mIconSize.height() + margins.bottom()
                );
}


/************************************************

 ************************************************/
void TrayIcon::setIconSize(QSize iconSize)
{
    mIconSize = iconSize;

    if (mWindowId)
        xfitMan().resizeWindow(mWindowId, mIconSize.width(), mIconSize.height());

    if (mIconId)
        xfitMan().resizeWindow(mIconId,   mIconSize.width(), mIconSize.height());
}


/************************************************

 ************************************************/
bool TrayIcon::event(QEvent *event)
{
    switch (event->type())
    {
        case QEvent::Paint:
            draw(static_cast<QPaintEvent*>(event));
            break;

        case QEvent::Resize:
            {
                QRect rect = iconGeometry();
                xfitMan().moveWindow(mWindowId, rect.left(), rect.top());
            }
            break;

            case QEvent::MouseButtonPress:
                event->accept();
                break;
         default:
            break;
    }

    return QFrame::event(event);
}


/************************************************

 ************************************************/
QRect TrayIcon::iconGeometry()
{
    QRect res = QRect(QPoint(0, 0), mIconSize);

    res.moveCenter(QRect(0, 0, width(), height()).center());
    return res;
}


/************************************************

 ************************************************/
void TrayIcon::draw(QPaintEvent* /*event*/)
{
    Display* dsp = QX11Info::display();

    XWindowAttributes attr;
    if (!XGetWindowAttributes(dsp, mIconId, &attr))
    {
        qWarning() << "Paint error";
        return;
    }

    XImage* ximage = XGetImage(dsp, mIconId, 0, 0, attr.width, attr.height, AllPlanes, ZPixmap);
    if (!ximage)
    {
        qWarning() << "    * Error image is NULL";
        return;
    }


//    qDebug() << "Paint icon **************************************";
//    qDebug() << "  * XComposite: " << isXCompositeAvailable();
//    qDebug() << "  * Icon geometry:" << iconGeometry();
//    qDebug() << "  Icon";
//    qDebug() << "    * window id:  " << hex << mIconId;
//    qDebug() << "    * window name:" << xfitMan().getName(mIconId);
//    qDebug() << "    * size (WxH): " << attr.width << "x" << attr.height;
//    qDebug() << "    * pos (XxY):  " << attr.x << attr.y;
//    qDebug() << "    * color depth:" << attr.depth;
//    qDebug() << "  XImage";
//    qDebug() << "    * size (WxH):  " << ximage->width << "x" << ximage->height;
//    switch (ximage->format)
//    {
//        case XYBitmap: qDebug() << "    * format:   XYBitmap"; break;
//        case XYPixmap: qDebug() << "    * format:   XYPixmap"; break;
//        case ZPixmap:  qDebug() << "    * format:   ZPixmap"; break;
//    }
//    qDebug() << "    * color depth:  " << ximage->depth;
//    qDebug() << "    * bits per pixel:" << ximage->bits_per_pixel;


    //const uchar* d =(uchar*) ximage->data;
    QImage image = QImage((const uchar*) ximage->data, ximage->width, ximage->height, ximage->bytes_per_line,  QImage::Format_ARGB32_Premultiplied);


    // Draw QImage ...........................
    QPainter painter(this);
    QRect iconRect = iconGeometry();
    if (image.size() != iconRect.size())
    {
        image = image.scaled(iconRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QRect r = image.rect();
        r.moveCenter(iconRect.center());
        iconRect = r;
    }
//    qDebug() << " Draw rect:" << iconRect;

    painter.drawImage(iconRect, image);

    XDestroyImage(ximage);
//    debug << "End paint icon **********************************";
}


/************************************************

 ************************************************/
bool TrayIcon::isXCompositeAvailable()
{
    int eventBase, errorBase;
    return XCompositeQueryExtension(QX11Info::display(), &eventBase, &errorBase );
}
