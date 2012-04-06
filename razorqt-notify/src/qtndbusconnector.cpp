#include "qtndbusconnector.h"
#include <QtDBus/QDBusConnection>
#include <QApplication>
#include <QMessageBox>
#include <QDBusError>
#include <stdexcept>

#include "qtnlog.h"
#include "notification.h"
#include "notificationhandler.h"

namespace
{
    static const int scDefaultTimeout = 3000 ;
    const QString g_scFreedesktopNotificationName = "org.freedesktop.Notifications";
}

class QtnDbusConnectorPrivate{
public:
    QtnDbusConnectorPrivate(){}
    NotificationHandler* m_pHandler;
    QApplication* m_pApp ;

    quint32 recalculateId(quint32 id) const
    {
        static quint32 currId = 1 ;

        quint32 newId = -1;
        if ( id == 0 )
        {
            newId = currId++ ;
            if ( currId >= 2048)
                currId=1;
        }
        else
            newId = id ;
        return newId ;
    }
};


QtnDbusConnector::QtnDbusConnector(QApplication* pA) :
    QDBusAbstractAdaptor(pA),
    d_ptr ( new QtnDbusConnectorPrivate)
{
    d_func()->m_pHandler= new NotificationHandler(this);
    d_func()->m_pApp = pA ;
}

void QtnDbusConnector::connectToDbus()
{
    static bool scbForceDisconnect = true ;

    QDBusConnection connection = QDBusConnection::connectToBus(QDBusConnection::SessionBus, g_scFreedesktopNotificationName);
    if ( scbForceDisconnect )
    {
        if (!connection.unregisterService(g_scFreedesktopNotificationName))
        {
            qDebug() << "Unable to unregistered previous service";
            // this is ok, we can still register
        }
    }

    if (!connection.registerService(g_scFreedesktopNotificationName))
    {
        qWarning() << "Cant register service " << g_scFreedesktopNotificationName << " error="
             << QDBusError::errorString(connection.lastError().type()).toStdString();
        throw std::logic_error("Cant register new service");
    }

    if (!connection.registerObject("/org/freedesktop/Notifications", d_func()->m_pApp, QDBusConnection::ExportAdaptors))
    {
        qWarning() << "Cant register object = " << QDBusError::errorString(connection.lastError().type()).toStdString();
        throw std::logic_error("Cant register object");
    }
}

QStringList QtnDbusConnector::GetCapabilities()
{
    QStringList lst;
    lst << "body" << "body-markup" << "body-hyperlinks" << "body-images"
        << "actions" << "icon-static" << "image/svg+xml" << "private-synchronous"
        << "append"  << "x-canonical-private-synchronous" << "x-canonical-private-icon-only";
    return lst;
}

QString QtnDbusConnector::GetServerInformation(QString &vendor, QString &version, QString &spec_version)
{
    vendor = "Razor-qt notification system";
    version = "0.2";
    spec_version = "0.2";
    return "";
}

quint32 QtnDbusConnector::Notify(QString app_name, unsigned id, QString icon, QString summary, QString body, QStringList actions, QVariantMap hints, int timeout)
{
    quint32 localid = id ;

    // if replace id is 0, then add some id
    if ( id == 0 )
        localid = d_func()->recalculateId(id);

    Notification notification = Notification(app_name, localid, icon, summary, body, actions, hints, timeout);
    d_func()->m_pHandler->addNotification(notification);

    return localid;
}

void QtnDbusConnector::show()
{
    d_func()->m_pHandler->showWindow() ;
}

void QtnDbusConnector::hide()
{
    d_func()->m_pHandler->hideWindow();
}

void QtnDbusConnector::CloseNotification(quint32 id)
{
    d_func()->m_pHandler->removeNotification(id);
}
