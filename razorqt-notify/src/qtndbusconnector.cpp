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
        if (connection.unregisterService(g_scFreedesktopNotificationName))
        {
            INFO("Unregistered previous service");
        }
        else
        {
            INFO("Unable to unregistered previous service");
            // this is ok, we can still register
        }
    }

    INFO("We're connected? isConnected()=" << connection.isConnected() );
    if (!connection.registerService(g_scFreedesktopNotificationName))
    {
        WARN("Cant register service " << g_scFreedesktopNotificationName << " error="
             << QDBusError::errorString(connection.lastError().type()).toStdString());
        throw std::logic_error("Cant register new service");
    }

    if (!connection.registerObject("/org/freedesktop/Notifications", d_func()->m_pApp, QDBusConnection::ExportAdaptors))
    {
        WARN("Cant register object = " << QDBusError::errorString(connection.lastError().type()).toStdString());
        throw std::logic_error("Cant register object");
    }
}

QStringList QtnDbusConnector::GetCapabilities()
{
    QStringList lst;
    lst << "body" << "body-markup" << "body-hyperlinks" << "body-images" << "actions" << "icon-static" << "image/svg+xml" << "private-synchronous" << "append"  << "x-canonical-private-synchronous";
    return lst;
}

QString QtnDbusConnector::GetServerInformation(QString &vendor, QString &version, QString &spec_version)
{
    vendor = "Qt notification daemon 2";
    version = "0.1";
    spec_version = "0.1";
    return "";
}

quint32 QtnDbusConnector::Notify(QString app_name, unsigned id, QString icon, QString summary, QString body, QStringList actions, QVariantMap hints, int timeout)
{
    TRACE("QtnDbusConnector::Notify app_name=" << app_name.toStdString() << " id =" << id << " summary=" << summary.toStdString() );
    quint32 localid = id ;
    localid = d_func()->recalculateId(id);

    if ( timeout == 0 )
        timeout = scDefaultTimeout;

    Notification* pN = d_func()->m_pHandler->findNotification( localid );
    if ( NULL == pN )
    {
        Notification notification = Notification(app_name, localid, icon, summary, body, actions, hints, timeout);
        d_func()->m_pHandler->addNotification(notification);
    }
    else
    {
        //FIXME
    }
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
    INFO("QtnDbusConnector::CloseNotification(unsigned id="<<id<<")");
    d_func()->m_pHandler->removeNotification(id);
}





