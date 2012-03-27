#include <QScopedPointer>

#include "notificationhandler.h"
#include "inotificationview.h"

#include "qmlnotification/notificationview.h"
#include "widgetnotification/widgetnotification.h"
#include "qtnlog.h"
#include "notificationserversettings.h"
#include "notificationtimeout.h"


class NotificationHandlerPrivate
{
public:
    struct NotificationInfo
    {
        NotificationInfo()
        {}
        Notification _notification ;
        QSharedPointer<NotificationTimeout> _pTimeout ;
    };

    NotificationHandlerPrivate()
    {}

    QScopedPointer<INotificationView> m_pView ;
    QList<NotificationInfo> m_notifications ;
};

NotificationHandler::NotificationHandler(QObject *parent) :
    QObject(parent),
    d_ptr(new NotificationHandlerPrivate)
{
    QString val = NotificationServerSettings::instance()->value("notification_type").toString();
    //TODO: add qml widget
//    if ( val == "qml" )
//        d_func()->m_pView = new NotificationView(this);
//    else if( val == "qwidget")
    d_func()->m_pView.reset( new WidgetNotification(this) );
}

NotificationHandler::~NotificationHandler()
{
    delete d_ptr ;
}

void NotificationHandler::addNotification(const Notification &pN)
{
    qDebug() << "Notification is about to be added." << pN;
    NotificationHandlerPrivate::NotificationInfo n ;
    n._notification = pN ;
    n._pTimeout = QSharedPointer<NotificationTimeout>(new NotificationTimeout);
    n._pTimeout->setNotification(n._notification);
    connect ( n._pTimeout.data(), SIGNAL(timeout()), this, SLOT(removeNotificationSlot()));
    d_func()->m_notifications.append(n);
    d_func()->m_pView->addNotification(pN);

    //FIXME: don't start timer here, as
    // handler don't know if notification is shown
    n._pTimeout->start();
}

Notification * NotificationHandler::findNotification(int id) const
{
    Notification* pN = NULL;
//    Q_FOREACH(NotificationShPtr shP, d_func()->m_notifications)
//    {
//        if ( shP->id() == id ){
//            pN = shP.data();
//            break ;
//        }
//    }

    return pN ;
}

void NotificationHandler::removeNotification(int id)
{
    INFO("About to remove notification with id=" << id );
    Q_FOREACH( NotificationHandlerPrivate::NotificationInfo info, d_func()->m_notifications )
    {
        if ( info._notification.id() == id )
        {
            d_func()->m_pView->removeNotification(info._notification);
            break ;
        }
    }
}

void NotificationHandler::showWindow()
{
    d_func()->m_pView->showNotification();
}

void NotificationHandler::hideWindow()
{
    d_func()->m_pView->hideNotification();
}

void NotificationHandler::removeNotificationSlot()
{
    NotificationTimeout* pT = qobject_cast<NotificationTimeout*> ( sender() );
    qDebug() << " Notification timeout id=" <<pT->notification().id();
    d_func()->m_pView->removeNotification(pT->notification());
}
