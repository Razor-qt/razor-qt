#include "razornotification.h"

#include <QDBusMessage>
#include <QDBusConnection>
#include <QVariantMap>
#include <QDebug>

namespace
{
    const QString g_scFreedesktopNotificationName = "org.freedesktop.Notifications";
    const QString g_scSignalName = "Notify";
}

class RazorNotificationPrivate
{
public:
    RazorNotificationPrivate():
        m_bus(QDBusConnection::sessionBus())
    {
    }

    QDBusConnection m_bus ;
    void notify( const QString& appName,quint32 replace_id,const QString& appIcon, const QString& summary, const QString& body,
                 const QVariantMap& hints, qint32 expire_timeout)
    {
        QDBusMessage m = QDBusMessage::createMethodCall(g_scFreedesktopNotificationName, "/org/freedesktop/Notifications", g_scFreedesktopNotificationName, "Notify");
        m << appName << replace_id << appIcon << summary << body << hints << expire_timeout ;
        QDBusMessage recive=QDBusConnection::sessionBus().call(m);
    }
};

RazorNotification::RazorNotification():
    d_ptr( new RazorNotificationPrivate)
{
}

void RazorNotification::notify(const QString &appName, quint32 replace_id, const QString &appIcon, const QString &summary, const QString &body, const QVariantMap &hints, qint32 expire_timeout)
{
    RazorNotification n ;
    n.d_func()->notify(appName, replace_id, appIcon, summary, body, hints,expire_timeout);
}
