#ifndef NOTIFICATIONTIMEOUT_H
#define NOTIFICATIONTIMEOUT_H

#include <QObject>

class Notification;
class QTimer ;

class NotificationTimeoutPrivate;

class NotificationTimeout : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(NotificationTimeout)
public:
    explicit NotificationTimeout() ;
    ~NotificationTimeout();
    const Notification& notification() const ;

    void setNotification( const Notification& n);
    void start();
    void stop();
private:
    NotificationTimeoutPrivate* const d_ptr ;

signals:
    void timeout() ;

};

#endif // NOTIFICATIONTIMEOUT_H
