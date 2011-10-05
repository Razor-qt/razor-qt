/* BEGIN_COMMON_COPYRIGHT_HEADER
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
#include "application.h"


/*! \mainpage razor-appswitcher, lightweight "alt+tab" application switcher

Some Window Managers (WM) do not provide well known feature how to change
active window with for example Alt+Tab key combination. In this case you
can use this Razor application (part of RazorQt project).

Using this Razor module is optional and it is not required in most cases
eg. in Openbox etc.
But you can find it useful in other minimalistic WMs
*/
int main(int argc, char **argv)
{
    RazorAppSwitcher::Application a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    a.setWindowIcon(QIcon(QString(SHARE_DIR) + "/graphics/razor_logo.png"));

    return a.exec();
}
