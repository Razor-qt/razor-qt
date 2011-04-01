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
