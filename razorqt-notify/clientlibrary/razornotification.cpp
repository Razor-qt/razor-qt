#include "razornotification.h"

#include <QDBusMessage>
#include <QDBusConnection>
#include <QVariantMap>

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
};

RazorNotification::RazorNotification():
    d_ptr( new RazorNotificationPrivate)
{
}

void RazorNotification::Notify(const QString &appName, quint32 replace_id, const QString &appIcon, const QString &summary, const QString &body, const QVariantMap &hints, qint32 expire_timeout)
{
    QDBusMessage m = QDBusMessage::createSignal("/",g_scFreedesktopNotificationName,g_scSignalName);
    m << appName << replace_id << appIcon << summary << body << hints << expire_timeout ;
    d_func()->m_bus.send(m);
}

void RazorNotification::NotifyS(const QString &appName, quint32 replace_id, const QString &appIcon, const QString &summary, const QString &body, const QVariantMap &hints, qint32 expire_timeout)
{
    RazorNotification n ;
    n.Notify(appName, replace_id, appIcon, summary, body, hints, expire_timeout);
}
