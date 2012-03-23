#ifndef NOTIFICATIONVIEW_H
#define NOTIFICATIONVIEW_H

#include <QWidget>
#include <QMap>

#include "inotificationview.h"

class QDeclarativeView ;
class Notification ;

class NotificationView : public INotificationView
{
    Q_OBJECT
public:
    explicit NotificationView(QObject* parent);
    ~NotificationView();

    virtual void show();
    virtual void hide() ;

    virtual void addToView( Notification* pN );
    virtual int viewCount() const ;

    virtual QWidget* widget() { return m_pMainWidget ; }


private slots:
    void removedFromQml( int index ) ;
private:
    QDeclarativeView* m_pQmlView ;
    QWidget* m_pMainWidget ;

private:
    void qmlAdd();
    void qmlRemove(Notification *pN);
    int  qmlCount() const;

    virtual void remove(Notification* pN) ;
};

#endif // NOTIFICATIONVIEW_H
