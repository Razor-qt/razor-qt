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


#include <QDebug>
#include <razorqt/xdgicon.h>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QContextMenuEvent>

#include "razortaskbutton.h"
#include "../xfitman2.h"
#include <QX11Info>

//%%%%%%%%%%%
//#include <X11/Xutil.h>
//#include <X11/Xatom.h>


#define DEFAULT_APP_ICON "xorg"
/************************************************

************************************************/
RazorTaskButton::RazorTaskButton(const Window window, QWidget *parent) :
    QToolButton(parent),
    mWindow(window)
{
    mButtonsGroup.addButton(this);
    mButtonsGroup.setExclusive(false);
    setCheckable(true);

    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setMaximumWidth(200);

    setText(xfitMan2()->getName(mWindow));

    QPixmap pix;
    if (xfitMan2()->getClientIcon(window, pix))
        setIcon(QIcon(pix));
    else
        setIcon(XdgIcon::fromTheme(DEFAULT_APP_ICON, 32));

    connect(this, SIGNAL(clicked(bool)), this, SLOT(btnClicked(bool)));


    XSelectInput(QX11Info::display(), mWindow, EnterWindowMask|FocusChangeMask|PropertyChangeMask|StructureNotifyMask);

//    XSelectInput(QX11Info::display(), mWindow, PropertyChangeMask| KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
//                                             KeymapStateMask | ButtonMotionMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask | FocusChangeMask |
//                                             VisibilityChangeMask | ExposureMask | StructureNotifyMask | SubstructureRedirectMask | SubstructureNotifyMask);


    XSetWindowAttributes attr;
    attr.event_mask = PropertyChangeMask;
    XChangeWindowAttributes(QX11Info::display(), QX11Info::appRootWindow(), CWEventMask, &attr);

}


/************************************************

************************************************/
RazorTaskButton::~RazorTaskButton()
{
}


/************************************************

 ************************************************/
void RazorTaskButton::nextCheckState()
{
    setChecked(xfitMan2()->getActiveAppWindow() == mWindow);
}


/************************************************

************************************************/
bool RazorTaskButton::isAppHidden() const
{
    return xfitMan2()->isHidden(mWindow);
}


/************************************************

************************************************/
bool RazorTaskButton::isApplicationActive() const
{
    return xfitMan2()->getActiveAppWindow() == mWindow;
}


/************************************************

************************************************/
void RazorTaskButton::btnClicked(bool checked)
{
    if (checked)
        minimizeApplication();
    else
        raiseApplication();
}


/************************************************

************************************************/
void RazorTaskButton::raiseApplication()
{
    xfitMan2()->raiseWindow(mWindow);
}


/************************************************

************************************************/
void RazorTaskButton::minimizeApplication()
{
    xfitMan2()->minimizeWindow(mWindow);
}


/************************************************

************************************************/
void RazorTaskButton::closeApplication()
{
    xfitMan2()->closeWindow(mWindow);
}


/************************************************

************************************************/
void RazorTaskButton::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(tr("Appliction"));
    QAction* a;

    /* KDE menu *******

      To &Desktop >
            &All Desktops
            ---
            &1 Desktop 1
            &2 Desktop 2
      &To Current Desktop
      &Move
      Re&size
      Mi&nimize
      Ma&ximize
      &Shade
      Ad&vanced >
            Keep &Above Others
            Keep &Below Others
      ---
      &Close
    */

    a = menu.addAction(tr("Mi&nimize"));
    //a->setCheckable(true);
    //a->setChecked();
    connect(a, SIGNAL(triggered(bool)), this, SLOT(minimizeApplication()));


    menu.addSeparator();
    a = menu.addAction(XdgIcon::fromTheme("process-stop", 22), tr("&Close"));
    connect(a, SIGNAL(triggered(bool)), this, SLOT(closeApplication()));
    menu.exec(mapToGlobal(event->pos()));
}

QButtonGroup RazorTaskButton::mButtonsGroup;
