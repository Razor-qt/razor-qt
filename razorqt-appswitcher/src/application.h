/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL3+
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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include "appswitcher.h"


namespace RazorAppSwitcher
{

/*! \brief This inherits QApplication and therefore does our X11 event-handling
 */
class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int & argc, char **argv);
    ~Application();

protected:
    bool x11EventFilter(XEvent * e);

private:
    RazorAppSwitcher::AppSwitcher * m_as;
};

}; // namespace


#endif
