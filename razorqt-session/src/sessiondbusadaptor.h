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

#ifndef SESSIONDBUS_H
#define SESSIONDBUS_H

#include <QtDBus>

#include "razormodman.h"


/*! \brief Simple DBus adaptor for RazorSession.
It allows 3rd party apps/razor modules to logout from session.
It's a part of "Razor Power Management" - see librazorqt.
\author Petr Vanek <petr@scribus.info>
*/
class SessionDBusAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.razor.session")

public:
    SessionDBusAdaptor(RazorModuleManager * manager)
        : QDBusAbstractAdaptor(manager),
          m_manager(manager)
    {
    }
        
public slots:
    Q_NOREPLY void logout()
    {
        m_manager->logout();
    }

private:
    RazorModuleManager * m_manager;
};

#endif
