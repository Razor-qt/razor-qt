#include <QScopedPointer>

#include "notificationhandler.h"
#include "inotificationview.h"
#include "razorsettings.h"

#include "qmlnotification/notificationview.h"
#include "widgetnotification/widgetnotification.h"
#include "qtnlog.h"
#include "notificationtimeout.h"

namespace
{
    const QString g_scNotificationPositionSetting = "notification_position";
    const QPoint g_scDefaultNotificationPosition = QPoint(100,100);

    const QString g_scNotificationSizeSetting = "notification_size";
    const QPoint g_scDefaultNotificationSize = QPoint(400,80);

    const QString g_scNotificationOpacitySetting = "notification_opacity";
    const qreal g_scDefaultNotificationOpacity= 0.7;

    const QString g_scNotificationBckColorSetting = "notification_backgroundColor";
    const QColor g_scDefaultNotificationBckColor=QColor(Qt::gray);
}


class NotificationHandlerPrivate
{
public:
    struct NotificationInfo
    {
        NotificationInfo()
        {}
        Notification _notification ;
        QSharedPointer<NotificationTimeout> _pTimeout ;
    };

    NotificationHandlerPrivate():
        m_settings("razorqt-notify")
    {}

    void createDefaultSettings()
    {
        // create default values if necessary

        // position
        QPoint position = m_settings.value(g_scNotificationPositionSetting).toPoint();
        if ( position.isNull() )
        {
            m_settings.setValue(g_scNotificationPositionSetting, g_scDefaultNotificationPosition);
        }

        // size
        QPoint settings = m_settings.value(g_scNotificationSizeSetting).toPoint();
        if ( settings.isNull() )
        {
            m_settings.setValue(g_scNotificationSizeSetting, g_scDefaultNotificationSize);
        }

        // opacity
        bool bOk=false;
        m_settings.value(g_scNotificationOpacitySetting).toReal(&bOk);
        if ( !bOk )
        {
            m_settings.setValue(g_scNotificationOpacitySetting, g_scDefaultNotificationOpacity);
        }

        // color

        QColor bckColor = m_settings.value(g_scNotificationBckColorSetting).value<QColor>();
        if ( !bckColor.isValid() )
        {
            m_settings.setValue(g_scNotificationBckColorSetting, g_scDefaultNotificationBckColor);
        }

        m_settings.sync();
    }

    QScopedPointer<INotificationView> m_pView ;
    QList<NotificationInfo> m_notifications ;
    RazorSettings m_settings ;
};

NotificationHandler::NotificationHandler(QObject *parent) :
    QObject(parent),
    d_ptr(new NotificationHandlerPrivate)
{
    QString val = d_func()->m_settings.value("notification_type").toString();
    //TODO: add qml widget
//    if ( val == "qml" )
//        d_func()->m_pView = new NotificationView(this);
//    else if( val == "qwidget")

    d_func()->createDefaultSettings();

    d_func()->m_pView.reset( new WidgetNotification(this) );
}

NotificationHandler::~NotificationHandler()
{
    delete d_ptr ;
}

void NotificationHandler::addNotification(const Notification &pN)
{
    qDebug() << "Notification is about to be added." << pN;
    NotificationHandlerPrivate::NotificationInfo n ;
    n._notification = pN ;
    if ( pN.timeout() == -1 )
    {
        qDebug() << " Persistent application!";
        // persistant notification, no timeout!
    }
    else
    {
        n._pTimeout = QSharedPointer<NotificationTimeout>(new NotificationTimeout);
        n._pTimeout->setNotification(n._notification);
        connect ( n._pTimeout.data(), SIGNAL(timeout()), this, SLOT(removeNotificationSlot()));

        //FIXME: don't start timer here, as
        // handler don't know if notification is shown
        n._pTimeout->start();
    }

    d_func()->m_notifications.append(n);
    d_func()->m_pView->addNotification(pN);


}

Notification * NotificationHandler::findNotification(quint32 id) const
{
    Notification* pN = NULL;
//    Q_FOREACH(NotificationShPtr shP, d_func()->m_notifications)
//    {
//        if ( shP->id() == id ){
//            pN = shP.data();
//            break ;
//        }
//    }

    return pN ;
}

void NotificationHandler::removeNotification(quint32 id)
{
    INFO("About to remove notification with id=" << id );
    bool bFound=false;
    Q_FOREACH( NotificationHandlerPrivate::NotificationInfo info, d_func()->m_notifications )
    {
        if ( info._notification.id() == id )
        {
            d_func()->m_pView->removeNotification(info._notification);
            bFound=true;
            break ;
        }
    }

    if (!bFound)
        qWarning() << " Unable to find proper notification with id=" << id ;
}

void NotificationHandler::showWindow()
{
    d_func()->m_pView->showNotification();
}

void NotificationHandler::hideWindow()
{
    d_func()->m_pView->hideNotification();
}

void NotificationHandler::removeNotificationSlot()
{
    NotificationTimeout* pT = qobject_cast<NotificationTimeout*> ( sender() );
    qDebug() << " Notification timeout id=" <<pT->notification().id();
    d_func()->m_pView->removeNotification(pT->notification());
}
