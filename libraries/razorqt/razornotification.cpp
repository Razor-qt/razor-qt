#include "razornotification.h"

#include <QDBusMessage>
#include <QDBusConnection>
#include <QVariantMap>
#include <QDebug>
#include <QStringList>


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
        QDBusMessage m = QDBusMessage::createMethodCall(QString::fromLatin1("org.freedesktop.Notifications"),
                                                        QString::fromLatin1("/org/freedesktop/Notifications"),
                                                        QString::fromLatin1("org.freedesktop.Notifications"),
                                                        QString::fromLatin1("Notify"));

        QList<QVariant> args;
        QStringList actions;

        args.append(appName); // app_name
        args.append( uint(-1) ); // replaces_id
        args.append(appIcon); // app_icon
        args.append(summary); // summary
        args.append(body); // body
        args.append(actions); // body

        args.append(hints); // hints
        args.append(expire_timeout); // timeout
        m.setArguments(args);
        QDBusMessage resp= QDBusConnection::sessionBus().call(m);
        qDebug() << resp  << resp.errorMessage();
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
