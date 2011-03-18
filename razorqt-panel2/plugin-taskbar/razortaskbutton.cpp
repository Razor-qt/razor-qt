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
#include <razorqt/xfitman.h>
#include <QX11Info>

#define MAX_BTN_WIDTH  400

/************************************************

************************************************/
RazorTaskButton::RazorTaskButton(const Window window, QWidget *parent) :
    QToolButton(parent),
    mWindow(window)
{
    //mButtonsGroup.addButton(this);
    //mButtonsGroup.setExclusive(false);
    setCheckable(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setMaximumWidth(MAX_BTN_WIDTH);

    updateText();
    updateIcon();

    connect(this, SIGNAL(clicked(bool)), this, SLOT(btnClicked(bool)));
    connect(this, SIGNAL(toggled(bool)), this, SLOT(checkedChanged(bool)));

    XSelectInput(QX11Info::display(), mWindow, EnterWindowMask|FocusChangeMask|PropertyChangeMask|StructureNotifyMask);

    XSetWindowAttributes attr;
    attr.event_mask = PropertyChangeMask;
    XChangeWindowAttributes(QX11Info::display(), QX11Info::appRootWindow(), CWEventMask, &attr);
}


/************************************************

************************************************/
RazorTaskButton::~RazorTaskButton()
{
    if (mCheckedBtn == this)
        mCheckedBtn = 0;
}


/************************************************

 ************************************************/
void RazorTaskButton::updateText()
{
    setText(xfitMan().getName(mWindow));
    setToolTip(text());
}


/************************************************

 ************************************************/
void RazorTaskButton::updateIcon()
{
    QPixmap pix;
    if (xfitMan().getClientIcon(mWindow, pix))
        setIcon(QIcon(pix));
    else
        setIcon(XdgIcon::defaultApplicationIcon());
}


/************************************************

 ************************************************/
void RazorTaskButton::nextCheckState()
{
    setChecked(xfitMan().getActiveAppWindow() == mWindow);
}


/************************************************

 ************************************************/
void RazorTaskButton::checkedChanged(bool checked)
{

    if (checked)
    {
        if (mCheckedBtn != 0 && mCheckedBtn != this)
            mCheckedBtn->setChecked(false);

        mCheckedBtn = this;
    }
}


/************************************************

 ************************************************/
void RazorTaskButton::unCheckAll()
{
    if (mCheckedBtn)
        mCheckedBtn->setChecked(false);
}


/************************************************

 ************************************************/
bool RazorTaskButton::isAppHidden() const
{
    return xfitMan().isHidden(mWindow);
}


/************************************************

 ************************************************/
bool RazorTaskButton::isApplicationActive() const
{
    return xfitMan().getActiveAppWindow() == mWindow;
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
    xfitMan().raiseWindow(mWindow);
}


/************************************************

 ************************************************/
void RazorTaskButton::minimizeApplication()
{
    xfitMan().minimizeWindow(mWindow);
}


/************************************************

 ************************************************/
void RazorTaskButton::maximizeApplication()
{
    xfitMan().maximizeWindow(mWindow);
}


/************************************************

 ************************************************/
void RazorTaskButton::shadeApplication()
{
    xfitMan().shadeWindow(mWindow, true);
}


/************************************************

 ************************************************/
void RazorTaskButton::unShadeApplication()
{
    xfitMan().shadeWindow(mWindow, false);
}


/************************************************

 ************************************************/
void RazorTaskButton::closeApplication()
{
    xfitMan().closeWindow(mWindow);
}


/************************************************

 ************************************************/
void RazorTaskButton::moveApplicationToDesktop()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if (!act)
        return;

    bool ok;
    int desk = act->data().toInt(&ok);

    if (!ok)
        return;

    xfitMan().moveWindowToDesktop(mWindow, desk);
}


/************************************************

 ************************************************/
void RazorTaskButton::contextMenuEvent(QContextMenuEvent* event)
{
    XfitMan xf = xfitMan();

    WindowAllowedActions allow = xf.getAllowedActions(mWindow);
    WindowState state = xf.getWindowState(mWindow);

    QMenu menu(tr("Appliction"));
    QAction* a;

    /* KDE menu *******

      + To &Desktop >
      +     &All Desktops
      +     ---
      +     &1 Desktop 1
      +     &2 Desktop 2
      + &To Current Desktop
        &Move
        Re&size
      + Mi&nimize
      + Ma&ximize
      + &Shade
        Ad&vanced >
            Keep &Above Others
            Keep &Below Others
            Fill screen
      ---
      + &Close
    */

    // ** Desktop menu **************************
    int deskNum = xf.getNumDesktop();
    if (deskNum > 1)
    {
        int winDesk = xf.getWindowDesktop(mWindow);
        QMenu* deskMenu = menu.addMenu(tr("To &Desktop"));

        a = deskMenu->addAction(tr("&All Desktops"));
        a->setData(-1);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(moveApplicationToDesktop()));
        deskMenu->addSeparator();

        for (int i=0; i<deskNum; ++i)
        {
            a = deskMenu->addAction(tr("Desktop &%1").arg(i+1));
            a->setData(i);
            a->setEnabled( i != winDesk );
            connect(a, SIGNAL(triggered(bool)), this, SLOT(moveApplicationToDesktop()));
        }

        int curDesk = xf.getActiveDesktop();
        a = menu.addAction(tr("&To Current Desktop"));
        a->setData(curDesk);
        a->setEnabled( curDesk != winDesk );
        connect(a, SIGNAL(triggered(bool)), this, SLOT(moveApplicationToDesktop()));
    }

    // ** State menu ****************************
    menu.addSeparator();

    a = menu.addAction(tr("Mi&nimize"));
    a->setEnabled(allow.Minimize && !state.Hidden);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(minimizeApplication()));

    a = menu.addAction(tr("Ma&ximize"));
    a->setEnabled((allow.MaximizeHoriz || allow.MaximizeVert) &&
                  (!state.MaximizedHoriz || !state.MaximizedVert || state.Hidden));
    connect(a, SIGNAL(triggered(bool)), this, SLOT(maximizeApplication()));

    if (state.Shaded)
    {
        a = menu.addAction(tr("Roll down"));
        a->setEnabled(allow.Shade && !state.Hidden);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(unShadeApplication()));
    }
    else
    {
        a = menu.addAction(tr("Roll up"));
        a->setEnabled(allow.Shade && !state.Hidden);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(shadeApplication()));
    }

    // ** Kill menu *****************************
    menu.addSeparator();
    a = menu.addAction(XdgIcon::fromTheme("process-stop", 22), tr("&Close"));
    connect(a, SIGNAL(triggered(bool)), this, SLOT(closeApplication()));
    menu.exec(mapToGlobal(event->pos()));
}


/************************************************

************************************************/
QSize RazorTaskButton::sizeHint() const
{
    QSize r = QToolButton::sizeHint();
    r.setWidth(40);
    return r;
}


/************************************************

************************************************/
void  RazorTaskButton::handlePropertyNotify(XPropertyEvent* event)
{
    if (event->atom == XfitMan::atom("WM_NAME") ||
        event->atom == XfitMan::atom("_NET_WM_VISIBLE_NAME"))
    {
        updateText();
        return;
    }

    if (event->atom == XfitMan::atom("_NET_WM_ICON"))
    {
        updateIcon();
        return;
    }


//    char* aname = XGetAtomName(QX11Info::display(), event->atom);
//    qDebug() << "** XPropertyEvent ********************";
//    qDebug() << "  atom:       0x" << hex << event->atom
//            << " (" << (aname ? aname : "Unknown") << ')';

}

RazorTaskButton* RazorTaskButton::mCheckedBtn = 0;

