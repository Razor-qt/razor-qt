#include "notificationtimeout.h"
#include "notification.h"
#include <QTimer>

class NotificationTimeoutPrivate
{
public:
    NotificationTimeoutPrivate():
        m_pNotification(NULL),
        m_pTimer( new QTimer)
    {
    }

    Notification* m_pNotification ;
    QTimer* m_pTimer ;
};

NotificationTimeout::NotificationTimeout(Notification* pN) :
    d_ptr( new NotificationTimeoutPrivate)
{
    d_func()->m_pNotification = pN ;
    d_func()->m_pTimer->setSingleShot(true);
    connect ( d_func()->m_pTimer, SIGNAL(timeout()), this, SIGNAL(timeout()));
}

Notification *NotificationTimeout::notification() const
{
    return d_func()->m_pNotification ;
}

void NotificationTimeout::start()
{
    d_func()->m_pTimer->start(d_func()->m_pNotification->timeout());
}

void NotificationTimeout::stop()
{
    d_func()->m_pTimer->stop();
}
