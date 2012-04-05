#include <QScopedPointer>
#include <QMutex>
#include <QMutexLocker>

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
    const QPoint g_scDefaultNotificationSize = QPoint(400,120);

    const QString g_scNotificationOpacitySetting = "notification_opacity";
    const qreal g_scDefaultNotificationOpacity= 0.9;

    const QString g_scNotificationBckColorSetting = "notification_backgroundColor";
    const QColor g_scDefaultNotificationBckColor=QColor(Qt::gray);

    const quint16 g_scDefaultTimeout = 3000 ;
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
    Notification parseNotification( const Notification& pN)
    {
        Notification ret = pN ;

        // check for urgency level
        QVariantMap hints = ret.hints();
        if ( hints.contains("urgency") )
        {
            quint8 urgLevel = hints["urgency"].toInt();
            qDebug() << "Found urgency hints" << urgLevel;
            if ( urgLevel == 2 )
            {
                qDebug() << "==========";
                // this has to be persistent notification
                ret.setTimeout(0);
            }
        }

        return ret ;
    }

    bool containsNotification(quint32 id) const
    {
        bool bRet=false ;
        for ( int i = 0 ; i < m_notifications.size(); ++i )
        {
            if ( m_notifications.at(i)._notification.id() == id )
            {
                bRet = true ;
            }
        }

        return bRet ;
    }

    NotificationInfo findNotification(quint32 id) const
    {
        for ( int i = 0 ; i < m_notifications.size(); ++i )
        {
            if ( m_notifications.at(i)._notification.id() == id )
            {
                return m_notifications[i];
            }
        }

        return NotificationInfo();
    }

    QScopedPointer<INotificationView> m_pView ;
    QList<NotificationInfo> m_notifications ;
    RazorSettings m_settings ;
    QMutex m_mutex ;
};

NotificationHandler::NotificationHandler(QObject *parent) :
    QObject(parent),
    d_ptr(new NotificationHandlerPrivate)
{
    d_func()->createDefaultSettings();


    QString val = d_func()->m_settings.value("notification_type").toString();
    //TODO: add qml widget
    if ( val == "qml" )
        d_func()->m_pView.reset( new QmlNotificationView() );
    else
        d_func()->m_pView.reset( new WidgetNotification(this) );

    connect ( d_func()->m_pView->notifier(), SIGNAL(notificationShowned(quint32)), this, SLOT(notificationShown(quint32)));
}

NotificationHandler::~NotificationHandler()
{
    delete d_ptr ;
}

void NotificationHandler::addNotification(const Notification &pN)
{
    QMutexLocker l ( &(d_func()->m_mutex ));
    qDebug() << "Notification is about to be added." << pN;

    if ( d_func()->containsNotification(pN.id()) )
    {
        NotificationHandlerPrivate::NotificationInfo pNotifInfo = d_func()->findNotification(pN.id());
        pNotifInfo._pTimeout->stop();
        pNotifInfo._pTimeout->start();
        pNotifInfo._notification = pN ;
        d_func()->m_pView->addNotification(pNotifInfo._notification);
    }
    else
    {
        NotificationHandlerPrivate::NotificationInfo n ;

        n._notification = d_func()->parseNotification(pN);
        if ( n._notification.timeout() == 0 )
        {
            qDebug() << " Persistent application!";
            // persistant notification, no timeout!
        }
        else
        {
            if( n._notification.timeout() == -1 )
            {
                n._notification.setTimeout(g_scDefaultTimeout);
            }

            n._pTimeout = QSharedPointer<NotificationTimeout>(new NotificationTimeout);
            n._pTimeout->setNotification(n._notification);
            connect ( n._pTimeout.data(), SIGNAL(timeout()), this, SLOT(removeNotificationSlot()));
        }
        d_func()->m_notifications.append(n);
        d_func()->m_pView->addNotification(n._notification);
    }
}


void NotificationHandler::removeNotification(quint32 id)
{
    qDebug() << "About to remove notification with id=" << id ;
    bool bFound=false;
    for (int i=0;i< d_func()->m_notifications.size();++i)
    {
        if ( d_func()->m_notifications.at(i)._notification.id() == id )
        {
            d_func()->m_pView->removeNotification(d_func()->m_notifications.at(i)._notification);
            d_func()->m_notifications.removeAt(i);
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
    QMutexLocker l ( &(d_func()->m_mutex ));
    NotificationTimeout* pT = qobject_cast<NotificationTimeout*> ( sender() );
    qDebug() << " Notification timeout id=" <<pT->notification().id();
    d_func()->m_pView->removeNotification(pT->notification());
}

void NotificationHandler::notificationShown(quint32 id)
{
    if ( d_func()->containsNotification(id) )
    {
        d_func()->findNotification(id)._pTimeout->start();
    }
}
