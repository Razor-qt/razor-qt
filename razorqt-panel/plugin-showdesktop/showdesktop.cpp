/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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

#include <QtGui/QToolButton>
#include <QtGui/QAction>
#include <QtGui/QX11Info>
#include <razor-global-key-shortcuts-client/razor-global-key-shortcuts-client.h>
#include <qtxdg/xdgicon.h>
#include <razorqt/xfitman.h>
#include <razorqt/razornotification.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>

#include "showdesktop.h"


Q_EXPORT_PLUGIN2(showdesktop, ShowDesktopLibrary)

#define DEFAULT_SHORTCUT "Control+Alt+D"


ShowDesktop::ShowDesktop(const IRazorPanelPluginStartupInfo &startupInfo) :
    QObject(),
    IRazorPanelPlugin(startupInfo)
{
    m_key = GlobalKeyShortcut::Client::instance()->addAction(QString(), QString("/panel/%1/show_desktop/%2/show_hide").arg(QFileInfo(settings()->fileName()).baseName()).arg(settings()->group()), tr("Show desktop"), this);
    if (m_key)
    {
        connect(m_key, SIGNAL(activated()), this, SLOT(showDesktop()));

        if (m_key->shortcut().isEmpty())
        {
            m_key->changeShortcut(DEFAULT_SHORTCUT);
            if (m_key->shortcut().isEmpty())
            {
                RazorNotification::notify(tr("Show Desktop: Global shortcut '%1' cannot be registered").arg(DEFAULT_SHORTCUT));
            }
        }
    }

    QAction * act = new QAction(XdgIcon::fromTheme("user-desktop"), tr("Show Desktop"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(showDesktop()));

    mButton.setDefaultAction(act);
    mButton.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
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
        XFree(data);
    }

    xfitMan().clientMessage(QX11Info::appRootWindow(),xfitMan().atom("_NET_SHOWING_DESKTOP"),(unsigned long) !bDesktopShown, 0,0,0,0);
}

#undef DEFAULT_SHORTCUT
