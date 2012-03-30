#include "notificationtimeout.h"
#include "notification.h"
#include <QTimer>

class NotificationTimeoutPrivate
{
public:
    NotificationTimeoutPrivate()
    {
    }

    Notification m_notification ;
    QTimer m_timer ;
};

NotificationTimeout::NotificationTimeout() :
    d_ptr( new NotificationTimeoutPrivate)
{
}
NotificationTimeout::~NotificationTimeout()
{
    stop();
    delete d_ptr ;
}

const Notification &NotificationTimeout::notification() const
{
    return d_func()->m_notification ;
}

void NotificationTimeout::setNotification(const Notification &n)
{
    d_func()->m_notification = n ;
    d_func()->m_timer.setSingleShot(true);
    connect ( &(d_func()->m_timer), SIGNAL(timeout()), this, SIGNAL(timeout()));
}

void NotificationTimeout::start()
{
    d_func()->m_timer.start(d_func()->m_notification.timeout());
}

void NotificationTimeout::stop()
{
    d_func()->m_timer.stop();
}
