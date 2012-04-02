#ifndef NOTIFICATIONVIEW_H
#define NOTIFICATIONVIEW_H

#include <QtDeclarative/QDeclarativeView>
#include <QtCore/QMap>

#include "inotificationview.h"

class Notification ;

class QmlNotificationView : public QDeclarativeView, public INotificationView
{
    Q_OBJECT
public:
    explicit QmlNotificationView(QWidget* parent = NULL);

    virtual void addNotification(const Notification& pN)  ;

    virtual void removeNotification( const Notification&  pN ) ;

    virtual QObject* notifier()  ;

    /**
     * @brief return number of currently displayed notifications
     */
    virtual int viewCount() const  ;

    /**
     * @brief show notification window
     *
     */
    virtual void showNotification()  ;

    /**
     * @brief hides notification window
     *
     */
    virtual void hideNotification()  ;
private:
};

#endif // NOTIFICATIONVIEW_H
