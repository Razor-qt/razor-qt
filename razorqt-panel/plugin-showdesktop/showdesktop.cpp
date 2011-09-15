/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include <QtGui/QToolButton>
#include <QtGui/QMessageBox>
#include <QtGui/QAction>
#include <QtGui/QX11Info>
#include <razorqxt/qxtglobalshortcut.h>
#include <qtxdg/xdgicon.h>
#include <razorqt/xfitman.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>

#include "showdesktop.h"


EXPORT_RAZOR_PANEL_PLUGIN_CPP(ShowDesktop)


ShowDesktop::ShowDesktop(const RazorPanelPluginStartInfo* startInfo, QWidget* parent)
    : RazorPanelPlugin(startInfo, parent)
{
    setObjectName("ShowDesktop");
    
    m_key = new QxtGlobalShortcut(this);

    QKeySequence ks(Qt::CTRL + Qt::ALT + Qt::Key_D);
    if (! m_key->setShortcut(ks))
    {
        QMessageBox::information(this, tr("Global keyboard shortcut"),
                                 tr("Panel Show Desktop Global shorcut: '%1' cannot be registered").arg(ks.toString()));
    }
    
    connect(m_key, SIGNAL(activated()), this, SLOT(showDesktop()));
    
    QAction * act = new QAction(XdgIcon::fromTheme("user-desktop"), tr("Show Desktop"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(showDesktop()));

    QToolButton * button = new QToolButton(this);
    button->setDefaultAction(act);
    addWidget(button);
}

void ShowDesktop::showDesktop()
{
    bool bDesktopShown=false;
    Atom actual_type;
    int actual_format, error;
    Atom _NET_SHOWING_DESKTOP = xfitMan().atom("_NET_SHOWING_DESKTOP" ) ; 
    unsigned char * data ; 
    unsigned long nitems, after;
    error = XGetWindowProperty(QX11Info::display(), QX11Info::appRootWindow(), _NET_SHOWING_DESKTOP, 0, 1, false, XA_CARDINAL,
                               &actual_type, &actual_format, &nitems, &after, &data);
    if ( error == Success && NULL != data) 
    {
        bDesktopShown = static_cast<bool>(data[0]);
    }
    
    xfitMan().clientMessage(QX11Info::appRootWindow(),xfitMan().atom("_NET_SHOWING_DESKTOP"),(unsigned long) !bDesktopShown, 0,0,0,0);
}
