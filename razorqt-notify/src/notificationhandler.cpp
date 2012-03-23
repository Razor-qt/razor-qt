#include "notificationhandler.h"
#include "inotificationview.h"

#include "qmlnotification/notificationview.h"
#include "widgetnotification/widgetnotification.h"
#include "qtnlog.h"
#include "notificationserversettings.h"

typedef QSharedPointer<Notification> NotificationShPtr ;

class NotificationHandlerPrivate
{
public:
    NotificationHandlerPrivate():
        m_pView( NULL )
    {}

    QPointer<INotificationView> m_pView ;
    QList<NotificationShPtr> m_notifications ;
};

NotificationHandler::NotificationHandler(QObject *parent) :
    QObject(parent),
    d_ptr(new NotificationHandlerPrivate)
{
    QString val = NotificationServerSettings::instance()->value("notification_type").toString();
    if ( val == "qml" )
        d_func()->m_pView = new NotificationView(this);
    else if( val == "qwidget")
        d_func()->m_pView = new WidgetNotification(this);
}

NotificationHandler::~NotificationHandler()
{
    delete d_ptr->m_pView ;
    delete d_ptr ;
}

void NotificationHandler::addNotification(Notification *pN)
{
    CONTEXT_PUSH("NotificationHandler");
    qDebug() << "Notification is about to be added." << *pN;
    d_func()->m_notifications.append(NotificationShPtr(pN));
    d_func()->m_pView->addNotification(pN);
    CONTEXT_POP;
}

Notification * NotificationHandler::findNotification(int id) const
{
    Notification* pN = NULL;
    Q_FOREACH(NotificationShPtr shP, d_func()->m_notifications)
    {
        if ( shP->id() == id ){
            pN = shP.data();
            break ;
        }
    }

    return pN ;
}

void NotificationHandler::removeNotification(int id)
{
    CONTEXT_PUSH("NotificationHandler");
    INFO("About to remove notification with id=" << id );
    d_func()->m_pView->remove(id);
    CONTEXT_POP;
}

void NotificationHandler::showWindow()
{
    d_func()->m_pView->show();
}

void NotificationHandler::hideWindow()
{
    d_func()->m_pView->hide();
}
