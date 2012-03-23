#ifndef INOTIFICATIONVIEW_H
#define INOTIFICATIONVIEW_H

#include <QObject>
#include <QMap>

class Notification ;
class NotificationTimeout ;

/**
 * @brief A Notification view interface that declares operations done on notifications
 **/
class INotificationView : public QObject
{
    Q_OBJECT
public:
    INotificationView(QObject* parent=NULL);
    virtual ~INotificationView();


    /**
     * @brief adds notification to View
     * @param pN notification. INotificationView will not free memory
     */
    void addNotification(Notification* pN);

    /**
     * @brief removes notification from view
     * @param id
     */
    void remove(int id);


    // virtual methods to override


    /**
     * @brief return number of currently displayed notifications
     */
    virtual int viewCount() const = 0 ;

    /**
     * @brief show notification window
     *
     */
    virtual void show() = 0 ;

    /**
     * @brief hides notification window
     *
     */
    virtual void hide() = 0 ;


private:
    QMap<int,Notification*> m_notif ; //! map of notification, with id as key
    QMap<int,NotificationTimeout*> m_notifTimeouts ; //! map of timers for notifications
protected:

    //! this can be called from derived classes, when structures holding Notification* needs to be cleaned
    void requireRemove( int id );

    /**
     * @brief add notification to actual view implementation. Must be reimplemented
     * @param pN notification to add
     */
    virtual void addToView( Notification* pN ) = 0 ;

    /**
     * @brief removes notification to actual view implementation. Must be reimplemented
     * @param pN
     */
    virtual void remove(Notification* pN) = 0 ;
private slots:
    void notificationTimeout() ;
signals:
    //! when notification is removed from view
    void notificationRemoved(int id);

    //! if view is hidden
    void viewHidden() ;
};

#endif // INOTIFICATIONVIEW_H
