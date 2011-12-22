/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *   Maciej Płaza <plaza.maciej@gmail.com>
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


#include <QApplication>

#include <QtCore/QDebug>
#include <QToolButton>
#include <QSettings>

#include "razortaskbar.h"
#include <qtxdg/xdgicon.h>
#include <razorqt/xfitman.h>
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
RazorTaskBar::RazorTaskBar(const RazorPanelPluginStartInfo* startInfo, QWidget* parent) :
    RazorPanelPlugin(startInfo, parent),
    mButtonStyle(Qt::ToolButtonTextBesideIcon),
    mShowOnlyCurrentDesktopTasks(false)
{
    setObjectName("TaskBar");

    mLayout = qobject_cast<QBoxLayout*>(layout());
    if (!mLayout)
        return;

    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    sp.setHorizontalStretch(1);
    sp.setVerticalStretch(1);
    setSizePolicy(sp);
    mLayout->addStretch();

    mRootWindow = QX11Info::appRootWindow();

    settigsChanged();
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
            if (buttonMaxWidth == -1)
            {
                btn->setMaximumWidth(btn->height());
            }
            else
            {
                btn->setMaximumWidth(buttonMaxWidth);
            }
            mButtonsHash.insert(wnd, btn);
            // -1 is here due the last stretchable item
            mLayout->insertWidget(layout()->count()-1, btn);
            // now I want to set higher stretchable priority for buttons
            // to suppress stretchItem (last item) default value which
            // will remove that anoying aggresive space at the end -- petr
            mLayout->setStretch(layout()->count()-2, 1);
        }
    }

    refreshButtonVisibility();

    activeWindowChanged();

}

/************************************************

 ************************************************/
void RazorTaskBar::refreshButtonVisibility()
{
    int curretDesktop = xfitMan().getActiveDesktop();
    QHashIterator<Window, RazorTaskButton*> i(mButtonsHash);
    while (i.hasNext())
    {
        i.next();
        i.value()->setHidden(mShowOnlyCurrentDesktopTasks &&
                             i.value()->desktopNum() != curretDesktop &&
                             i.value()->desktopNum() != -1
                            );
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
            if (mShowOnlyCurrentDesktopTasks)
                refreshTaskList();
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

void RazorTaskBar::setButtonMaxWidth(int maxWidth)
{
   QHash<Window, RazorTaskButton*>::const_iterator i = mButtonsHash.constBegin();
   while (i != mButtonsHash.constEnd())
   {
       if (maxWidth == -1)
       {
           i.value()->setMaximumWidth(i.value()->height());
       }
       else
       {
           i.value()->setMaximumWidth(maxWidth);
       }
       ++i;
   }
}

/************************************************

 ************************************************/
void RazorTaskBar::settigsChanged()
{
    buttonMaxWidth = settings().value("maxWidth", 400).toInt();
    QString s = settings().value("buttonStyle").toString().toUpper();

    if (s == "ICON")
    {
        setButtonStyle(Qt::ToolButtonIconOnly);
        buttonMaxWidth = -1;
        setButtonMaxWidth(buttonMaxWidth);
    }
    else if (s == "TEXT")
    {
        setButtonStyle(Qt::ToolButtonTextOnly);
        setButtonMaxWidth(buttonMaxWidth);
    }
    else
    {
        setButtonStyle(Qt::ToolButtonTextBesideIcon);
        setButtonMaxWidth(buttonMaxWidth);
    }

    mShowOnlyCurrentDesktopTasks = settings().value("showOnlyCurrentDesktopTasks", mShowOnlyCurrentDesktopTasks).toBool();
    refreshTaskList();
}

void RazorTaskBar::showConfigureDialog()
{
    RazorTaskbarConfiguration *confWindow = this->findChild<RazorTaskbarConfiguration*>("TaskbarConfigurationWindow");

    if (!confWindow)
    {
        confWindow = new RazorTaskbarConfiguration(settings(), this);
    }

    confWindow->show();
    confWindow->raise();
    confWindow->activateWindow();
}
