#ifndef INOTIFICATIONVIEW_H
#define INOTIFICATIONVIEW_H

#include <QObject>
#include <QMap>

class Notification ;
class NotificationTimeout ;

/**
 * @brief A Notification view interface that declares operations done on notifications
 **/
class INotificationView
{
public:
    virtual ~INotificationView(){}

    /**
     * @brief adds notification to View
     * @param pN notification. INotificationView will not free memory
     */
    virtual void addNotification(const Notification& pN) = 0 ;

    virtual void removeNotification( const Notification&  pN ) = 0 ;

    virtual QObject* notifier() = 0 ;

    /**
     * @brief return number of currently displayed notifications
     */
    virtual int viewCount() const = 0 ;

    /**
     * @brief show notification window
     *
     */
    virtual void showNotification() = 0 ;

    /**
     * @brief hides notification window
     *
     */
    virtual void hideNotification() = 0 ;

};

#endif // INOTIFICATIONVIEW_H
