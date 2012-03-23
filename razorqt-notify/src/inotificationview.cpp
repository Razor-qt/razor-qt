#include "inotificationview.h"
#include "notification.h"
#include "notificationtimeout.h"

#include "qtnlog.h"

INotificationView::INotificationView(QObject* parent):
    QObject(parent)
{
    TRACE("Creating INotificationView");
}

INotificationView::~INotificationView()
{
    TRACE("Deleting INotificationView");
}

void INotificationView::addNotification(Notification *pN)
{
    if ( pN->timeout() == -1 ){
        // notification is persistent
    }
    NotificationTimeout* pT = new NotificationTimeout(pN);
    connect( pT, SIGNAL(timeout()), this, SLOT(notificationTimeout()));
    pT->start();
    show();
    m_notif[pN->id()] = pN ;
    m_notifTimeouts[pN->id()] = pT ;

    addToView(pN);
}

void INotificationView::notificationTimeout()
{
    NotificationTimeout* pT = qobject_cast<NotificationTimeout*>( sender() );
    Notification* pN = pT->notification();
    INFO("Timeout for pN->id()=" << pN->id());
    remove(pN);
    m_notif.remove(pN->id());
    emit notificationRemoved(pN->id());
}

void INotificationView::requireRemove(int id)
{
    m_notif.remove(id);
    if ( m_notifTimeouts.contains(id))
    {
        m_notifTimeouts[id]->stop();
        delete m_notifTimeouts[id];
        m_notifTimeouts.remove(id);
    }
    emit notificationRemoved(id);

    int c = viewCount();
    qDebug() << "Total notification count" << c ;
    if ( c == 0 )
    {
        qDebug() << " Hide notification window";
        hide();
    }
    emit viewHidden();
}

void INotificationView::remove(int id)
{
    if ( m_notif.contains(id))
    {
        remove( m_notif[id]);
        requireRemove(id);
    }
    else
    {
        INFO("Oups. Id= " << id << " is no longer valid ");
    }
}
