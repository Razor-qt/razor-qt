/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
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

#include <QtGui/QApplication>
#include <QtGui/QX11Info>
#include <QtGui/QDesktopWidget>
#include <QtDebug>

#include <razorqt/xfitman.h>
#include <X11/Xlib.h>

#ifndef RAZOR_VERSION
#define RAZOR_VERSION "Version n/a. Probably built out of Razor-qt tree"
#endif

/*! \mainpage X11 Info for Razor-qt.
 * This tool allows to gather various useful X11/screen information.
 * Sometimes it would be useful to attach output of this command line
 * application to the bugreport.
 *
 * \note This program is not useful for end-users. It's a helper
 *       for Razor development.
 */
int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    qDebug() << "Razor-qt (" << RAZOR_VERSION << ") environment summary\n";

#ifdef Q_WS_X11
    // collect QX11
    qDebug() << "* X11 Information";
    QX11Info x11;
    qDebug() << "X11 display:\t\t" << x11.display();
    qDebug() << "X11 colormap:\t\t" << x11.colormap();
    qDebug() << "X11 depth:\t\t" << x11.depth();
    qDebug() << "X11 composite:\t\t" << x11.isCompositingManagerRunning();
    qDebug() << "X11 ScreenCount:\t" << ScreenCount(QX11Info::display());
    qDebug() << "";
#endif

    qDebug() << "* QDesktopWidget Information";
    QDesktopWidget *dw = QApplication::desktop();
    qDebug() << "QD virtual desktop:\t" << dw->isVirtualDesktop();
    qDebug() << "QD primary screen:\t" << dw->primaryScreen();
    qDebug() << "QD screen count:\t" << dw->screenCount();
    qDebug() << "** QD screens";
    for (int i = 0; i < dw->screenCount(); ++i)
    {
        qDebug() << "\tScreen" << i << "availableGeometry:\t" << dw->availableGeometry(i);
        qDebug() << "\tScreen" << i << "screenGeometry:\t" << dw->screenGeometry(i);
        qDebug() << "";
    }
    
    qDebug() << "* XfitMan information";
    qDebug() << "getActiveDesktop:\t" << xfitMan().getActiveDesktop();
    qDebug() << "getNumDesktop:\t" << xfitMan().getNumDesktop();

    return 0;
}

