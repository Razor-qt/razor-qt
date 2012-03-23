#include "qtndbusconnector.h"
#include <QtDBus/QDBusConnection>
#include <QApplication>
#include <QMessageBox>
#include <QDBusError>

#include "qtnlog.h"
#include "notification.h"
#include "notificationhandler.h"

#include <stdio.h>

namespace {
    static const int scDefaultTimeout = 3000 ;
}

class QtnDbusConnectorPrivate{
public:
    QtnDbusConnectorPrivate(){}
    NotificationHandler* m_pHandler;
    QApplication* m_pApp ;
    int recalculateId(int id) const
    {
        static int currId = 1 ;

        int newId = -1;
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

    QDBusConnection connection = QDBusConnection::connectToBus(QDBusConnection::SessionBus, "org.freedesktop.Notifications");
    if ( scbForceDisconnect ){

        if (connection.unregisterService("org.freedesktop.Notifications")){
            INFO("Unregistered previous service");
        }
        else
            INFO("Unable to unregister previous service");
    }

//    if ( connection.isConnected() )
    INFO("We're connected? isConnected()=" << connection.isConnected() );
    if (!connection.registerService("org.freedesktop.Notifications"))
    {
        WARN("Cant register service= " << QDBusError::errorString(connection.lastError().type()).toStdString());
    }
    if (!connection.registerObject("/org/freedesktop/Notifications", d_func()->m_pApp, QDBusConnection::ExportAdaptors))
    {
        WARN("Cant register object = " << QDBusError::errorString(connection.lastError().type()).toStdString());
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

unsigned QtnDbusConnector::Notify(QString app_name, unsigned id, QString icon, QString summary, QString body, QStringList actions, QVariantMap hints, int timeout)
{
    TRACE("QtnDbusConnector::Notify app_name=" << app_name.toStdString() << " id =" << id << " summary=" << summary.toStdString() );
    int localid = id ;
    localid = d_func()->recalculateId(id);

    if ( timeout <= 0 )
        timeout = scDefaultTimeout;

    Notification* pN = d_func()->m_pHandler->findNotification( localid );
    if ( NULL == pN ){
        Notification* pN = new Notification(app_name, localid, icon, summary, body, actions, hints, timeout);
        d_func()->m_pHandler->addNotification(pN);
    }
    else
    {}
}

void QtnDbusConnector::show()
{
    d_func()->m_pHandler->showWindow() ;
}

void QtnDbusConnector::hide()
{
    d_func()->m_pHandler->hideWindow();
}


void QtnDbusConnector::CloseNotification(unsigned id)
{
    INFO("QtnDbusConnector::CloseNotification(unsigned id)")
}





