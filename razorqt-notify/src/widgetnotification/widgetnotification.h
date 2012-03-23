#ifndef WIDGETNOTIFICATION_H
#define WIDGETNOTIFICATION_H

#include "inotificationview.h"

#include <QRect>

class QStackedWidget ;
class Notification ;
class WidgetNotificationPrivate;

class WidgetNotification : public INotificationView
{
    Q_OBJECT

public:
    explicit WidgetNotification(QObject* parent);
    ~WidgetNotification();
signals:
    
public slots:
private:
    virtual int viewCount() const  ;
    virtual void show() ;
    virtual void hide()  ;

    virtual void addToView( Notification* pN )  ;
    virtual void remove(Notification* pN) ;
private:
    Q_DECLARE_PRIVATE(WidgetNotification)
    WidgetNotificationPrivate* const d_ptr ;
    
};

#endif // WIDGETNOTIFICATION_H
