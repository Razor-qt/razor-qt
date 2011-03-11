/********************************************************************
  Copyright: 2011 Alexander Sokoloff <sokoloff.a@gmail.ru>

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
#include <QApplication>

//#include "razortaskbar.h"

#include <QtCore/QDebug>
#include <QToolButton>

#include "razortaskbar.h"
#include <razorqt/xdgicon.h>
#include "../xfitman2.h"
#include <QtCore/QList>


#include <QDesktopWidget>

#include <X11/Xlib.h>
#include "razortaskbutton.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include <QX11Info>

EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorTaskBar)

/************************************************

************************************************/
RazorTaskBar::RazorTaskBar(RazorPanel* panel, const QString& configId, QWidget *parent) :
    RazorPanelPlugin(panel, configId, parent)
{
    setObjectName("TaskBar");
    setWindowTitle(tr("Task Bar"));

    connect(panel, SIGNAL(x11PropertyNotify(XEvent*)), this, SLOT(handleXEvent(XEvent*)));

    mRootWindow = QX11Info::appRootWindow();
    refreshTaskList();
}


/************************************************

 ************************************************/
RazorTaskBar::~RazorTaskBar()
{
}


/************************************************

 ************************************************/
RazorTaskButton* RazorTaskBar::buttonByWindow(Window window) const
{
    if (mButtonsHash.contains(window))
        return mButtonsHash.value(window);
    return 0;
}


/************************************************

 ************************************************/
void RazorTaskBar::refreshTaskList()
{
    XfitMan2 xf = xfitMan2();
    QList<Window> tmp = xf.getClientList();

    //qDebug() << "** Fill ********************************";
    //foreach (Window wnd, tmp)
    //    if (xf->acceptWindow(wnd)) qDebug() << XfitMan2::debugWindow(wnd);
    //qDebug() << "****************************************";


    QMutableHashIterator<Window, RazorTaskButton*> i(mButtonsHash);
    while (i.hasNext())
    {
        i.next();
        int n = tmp.removeAll(i.key());

        if (!n)
        {
            delete i.value();
            i.remove();
        }
    }

    foreach (Window wnd, tmp)
    {
        if (xf.acceptWindow(wnd))
        {
            RazorTaskButton* btn = new RazorTaskButton(wnd, this);
            mButtonsHash.insert(wnd, btn);
            addWidget(btn);
        }
    }

    activeWindowChanged();

}


/************************************************

 ************************************************/
void RazorTaskBar::activeWindowChanged()
{
    Window window = xfitMan2().getActiveAppWindow();

    RazorTaskButton* btn = buttonByWindow(window);

    if (btn)
        btn->setChecked(true);
    else
        RazorTaskButton::unCheckAll();
}


/************************************************

 ************************************************/
void RazorTaskBar::handleXEvent(XEvent* event)
{

    switch (event->type)
    {
        case PropertyNotify:
            handlePropertyNotify(&event->xproperty);
            break;

//        default:
//        {
//            qDebug() << "** XEvent ************************";
//            qDebug() << "Type:   " << xEventTypeToStr(event);
//        }


    }
}


/************************************************

 ************************************************/
void RazorTaskBar::handlePropertyNotify(XPropertyEvent* event)
{

    if (event->window == mRootWindow)
    {
        // Windows list changed ...............................
        if (event->atom == XfitMan2::atom("_NET_CLIENT_LIST"))
        {
            refreshTaskList();
            return;
        }

        // Activate window ....................................
        if (event->atom == XfitMan2::atom("_NET_ACTIVE_WINDOW"))
        {
            activeWindowChanged();
            return;
        }
    }
    else
    {
        RazorTaskButton* btn = buttonByWindow(event->window);
        if (btn)
            btn->handlePropertyNotify(event);
    }
//    char* aname = XGetAtomName(QX11Info::display(), event->atom);
//    qDebug() << "** XPropertyEvent ********************";
//    qDebug() << "  atom:       0x" << hex << event->atom
//            << " (" << (aname ? aname : "Unknown") << ')';
//    qDebug() << "  window:    " << XfitMan2::debugWindow(event->window);
//    qDebug() << "  display:   " << event->display;
//    qDebug() << "  send_event:" << event->send_event;
//    qDebug() << "  serial:    " << event->serial;
//    qDebug() << "  state:     " << event->state;
//    qDebug() << "  time:      " << event->time;
//    qDebug();

}


