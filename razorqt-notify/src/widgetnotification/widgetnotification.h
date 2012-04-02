#ifndef WIDGETNOTIFICATION_H
#define WIDGETNOTIFICATION_H

#include "inotificationview.h"
#include <QWidget>

class QStackedWidget ;
class Notification ;
class WidgetNotificationPrivate;

/**
 * @brief A notification that uses plain C++ widget creation as oposed to use qml
 **/
class WidgetNotification : public QWidget, public INotificationView
{
    Q_OBJECT
public:
    explicit WidgetNotification(QObject* parent);
    ~WidgetNotification();
    virtual void addNotification(const Notification& pN) ;
    virtual void removeNotification( const Notification&  pN ) ;
    virtual void showNotification() ;
    virtual void hideNotification()  ;
    virtual QObject* notifier();
    virtual int viewCount() const  ;
private:
    virtual void addToView( const Notification&  pN )  ;
private slots:
    void settingsChanged();
    void dismissNotification();
private:
    Q_DECLARE_PRIVATE(WidgetNotification)
    WidgetNotificationPrivate* const d_ptr ;
};

#endif // WIDGETNOTIFICATION_H
