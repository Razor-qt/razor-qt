/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *   Maciej Płaza <plaza.maciej@gmail.com>
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


#include <QApplication>

#include <QtCore/QDebug>
#include <QToolButton>
#include <QSettings>

#include "razortaskbar.h"
#include <razorqt/razorgridlayout.h>
#include <qtxdg/xdgicon.h>
#include <razorqt/xfitman.h>
#include <QtCore/QList>


#include <QDesktopWidget>
#include <QWheelEvent>

#include <X11/Xlib.h>
#include "razortaskbutton.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include <QX11Info>

#include <QDebug>
#include "../panel/irazorpanelplugin.h"

/************************************************

************************************************/
RazorTaskBar::RazorTaskBar(IRazorPanelPlugin *plugin, QWidget *parent) :
    QFrame(parent),
    mButtonStyle(Qt::ToolButtonTextBesideIcon),
    mShowOnlyCurrentDesktopTasks(false),
    mPlugin(plugin)
{
    setMinimumWidth(100);
    mLayout = new RazorGridLayout(this);
    setLayout(mLayout);
    mLayout->setMargin(0);
    realign();

    mRootWindow = QX11Info::appRootWindow();

    settingsChanged();
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
bool RazorTaskBar::windowOnActiveDesktop(Window window) const
{
    if (!mShowOnlyCurrentDesktopTasks)
        return true;

    XfitMan xf = xfitMan();
    int desktop = xf.getWindowDesktop(window);
    if (desktop == -1) // Show on all desktops
        return true;

    if (desktop == xf.getActiveDesktop())
        return true;

    return false;
}

/************************************************

 ************************************************/
void RazorTaskBar::refreshTaskList()
{
    XfitMan xf = xfitMan();
    QList<Window> tmp = xf.getClientList();

    //qDebug() << "** Fill ********************************";
    //foreach (Window wnd, tmp)
    //    if (xf->acceptWindow(wnd)) qDebug() << XfitMan::debugWindow(wnd);
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
            btn->setToolButtonStyle(mButtonStyle);

            mButtonsHash.insert(wnd, btn);
            mLayout->addWidget(btn);
        }
    }
    setButtonSizeLimits();
    refreshButtonVisibility();
    mLayout->invalidate();
    activeWindowChanged();
    realign();
}

/************************************************

 ************************************************/
void RazorTaskBar::refreshButtonVisibility()
{
    QHashIterator<Window, RazorTaskButton*> i(mButtonsHash);
    while (i.hasNext())
    {
        i.next();
        i.value()->setHidden(!windowOnActiveDesktop(i.key()));
    }
}


/************************************************

 ************************************************/
void RazorTaskBar::activeWindowChanged()
{
    Window window = xfitMan().getActiveAppWindow();

    RazorTaskButton* btn = buttonByWindow(window);

    if (btn)
        btn->setChecked(true);
    else
        RazorTaskButton::unCheckAll();
}


/************************************************

 ************************************************/
void RazorTaskBar::x11EventFilter(XEvent* event)
{
    switch (event->type)
    {
        case PropertyNotify:
            handlePropertyNotify(&event->xproperty);
            break;

#if 0
        case MotionNotify:
            break;

        default:
        {
            qDebug() << "** XEvent ************************";
            qDebug() << "Type:   " << xEventTypeToStr(event);
        }
#endif

    }
}


/************************************************

 ************************************************/
void RazorTaskBar::handlePropertyNotify(XPropertyEvent* event)
{
    if (event->window == mRootWindow)
    {
        // Windows list changed ...............................
        if (event->atom == XfitMan::atom("_NET_CLIENT_LIST"))
        {
            refreshTaskList();
            return;
        }

        // Activate window ....................................
        if (event->atom == XfitMan::atom("_NET_ACTIVE_WINDOW"))
        {
            activeWindowChanged();
            return;
        }

        // Desktop switch .....................................
        if (event->atom == XfitMan::atom("_NET_CURRENT_DESKTOP"))
        {
            if (mShowOnlyCurrentDesktopTasks)
                refreshTaskList();
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
//    qDebug() << "  window:    " << XfitMan::debugWindow(event->window);
//    qDebug() << "  display:   " << event->display;
//    qDebug() << "  send_event:" << event->send_event;
//    qDebug() << "  serial:    " << event->serial;
//    qDebug() << "  state:     " << event->state;
//    qDebug() << "  time:      " << event->time;
//    qDebug();

}


/************************************************

 ************************************************/
void RazorTaskBar::setButtonStyle(Qt::ToolButtonStyle buttonStyle)
{
    mButtonStyle = buttonStyle;

    QHashIterator<Window, RazorTaskButton*> i(mButtonsHash);
    while (i.hasNext())
    {
        i.next();
        i.value()->setToolButtonStyle(mButtonStyle);
    }
}


/************************************************

 ************************************************/
void RazorTaskBar::setButtonSizeLimits()
{
    QSize maxSize;
    QSize minSize = QSize(0, 0);
    
    if (mPlugin->panel()->isHorizontal())
    {
        minSize.setHeight(mPlugin->panel()->lineSize());
    }
    
    // max size fro icon only is disabled. And teh sizing is really
    // suboptimal for this stype without hard-setting the button to square
    // size. 20130316 pvanek
    if (mButtonStyle == Qt::ToolButtonIconOnly)
    {
        minSize.setWidth(minSize.height());
        maxSize = minSize;
    }
    else
    {
        // rest of toolbutton styles
        maxSize = QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        if (mPlugin->panel()->isHorizontal() &&
            mButtonMaxWidth > -1)
        {
            maxSize.setWidth(mButtonMaxWidth);
        }
    }

    QHash<Window, RazorTaskButton*>::const_iterator i = mButtonsHash.constBegin();

    while (i != mButtonsHash.constEnd())
    {
        i.value()->setMinimumSize(minSize);
        i.value()->setMaximumSize(maxSize);
        ++i;
    }

}

/************************************************

 ************************************************/
void RazorTaskBar::settingsChanged()
{
    mButtonMaxWidth = mPlugin->settings()->value("maxWidth", 400).toInt();
    QString s = mPlugin->settings()->value("buttonStyle").toString().toUpper();

    if (s == "ICON")
    {
        setButtonStyle(Qt::ToolButtonIconOnly);
        mButtonMaxWidth = -1;
        setButtonSizeLimits();
    }
    else if (s == "TEXT")
    {
        setButtonStyle(Qt::ToolButtonTextOnly);
        setButtonSizeLimits();
    }
    else
    {
        setButtonStyle(Qt::ToolButtonTextBesideIcon);
        setButtonSizeLimits();
    }

    mShowOnlyCurrentDesktopTasks = mPlugin->settings()->value("showOnlyCurrentDesktopTasks", mShowOnlyCurrentDesktopTasks).toBool();
    RazorTaskButton::setShowOnlyCurrentDesktopTasks(mShowOnlyCurrentDesktopTasks);
    RazorTaskButton::setCloseOnMiddleClick(mPlugin->settings()->value("closeOnMiddleClick", true).toBool());
    refreshTaskList();
}


/************************************************

 ************************************************/
void RazorTaskBar::realign()
{
    mLayout->setEnabled(false);
    IRazorPanel *panel = mPlugin->panel();
    setButtonSizeLimits();
    if (panel->isHorizontal())
    {
        mLayout->setRowCount(panel->lineCount());
        mLayout->setColumnCount(0);
        mLayout->setStretch(RazorGridLayout::StretchHoriz | RazorGridLayout::StretchVert);
    }
    else
    {
        if (mButtonStyle == Qt::ToolButtonIconOnly)
            mLayout->setColumnCount(panel->lineCount());
        else
            mLayout->setColumnCount(1);

        mLayout->setRowCount(0);
        mLayout->setStretch(RazorGridLayout::StretchHoriz);
    }
    mLayout->setEnabled(true);
}


/************************************************

 ************************************************/
void RazorTaskBar::wheelEvent(QWheelEvent* event)
{
    XfitMan xf = xfitMan();
    QList<Window> winList = xf.getClientList();
    int current = winList.indexOf(xf.getActiveAppWindow());
    int delta = event->delta() < 0 ? 1 : -1;

    for (int ix = current + delta; 0 <= ix && ix < winList.size(); ix += delta)
    {
        Window window = winList.at(ix);
        if (xf.acceptWindow(window) && windowOnActiveDesktop(window))
        {
            xf.raiseWindow(window);
            break;
        }
    }
}
