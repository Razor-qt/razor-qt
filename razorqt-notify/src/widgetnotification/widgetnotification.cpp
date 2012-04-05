#include "widgetnotification.h"
#include "notification.h"
#include "qtnlog.h"

#include "ui_notification.h"
#include "ui_normalNotification.h"
#include "ui_notificationWithProgress.h"
#include "razorsettings.h"
#include "widgetnotificationnotifier.h"
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QFileSystemWatcher>
#include <QDebug>

namespace
{
    static int scCornerNumber = 4;
    static int scRadiusSize = 10 ;
    static int scShadowSize = 13 ;
    static const int scIconSize = 64 ;
}

class WidgetNotificationPrivate : public QObject
{
public:

    enum NotificationType
    {
        eNormalNotification=0,
        eProgressBarNotification,
        eIconOnlyNotification
    };

    WidgetNotificationPrivate( WidgetNotification* pParent ):
        m_pNot( NULL ),
        m_bShowing(true),
        m_settings("razorqt-notify"),
        m_currentNotification(0),
        m_bEnableCompositing(true),
        m_widgetNotifier( new WidgetNotificationNotifier)
    {
        QImage shadow_edge(":osd_shadow_edge.png");
        QImage shadow_corner(":osd_shadow_corner.png");
        for ( int i = 0 ; i < scCornerNumber; ++i )
        {
            QTransform rotation = QTransform().rotate(90 * i);
            shadowsEdges.push_back(QPixmap::fromImage(shadow_edge.transformed(rotation)));
            shadowsCorners.push_back(QPixmap::fromImage(shadow_corner.transformed(rotation)));
        }
    }

    QRect geom() const
    {
        QPoint p = m_settings.value("notification_position").toPoint();
        QPoint size = m_settings.value("notification_size").toPoint();
        return QRect(p.x(),p.y(),size.x(),size.y());
    }

    bool eventFilter(QObject *pO, QEvent *event)
    {
        if ( event->type() == QEvent::Paint)
        {
            QWidget* pW = qobject_cast<QWidget*>(pO);

            QColor backColor = m_settings.value("notification_backgroundColor").value<QColor>();
            float fOpacity = m_settings.value("notification_opacity").value<float>();
            QPainter painter ( pW );

            painter.setRenderHint(QPainter::Antialiasing);
            painter.setRenderHint(QPainter::HighQualityAntialiasing);

            // corners
            const int kShadowCornerSize = scShadowSize + scRadiusSize;
            painter.drawPixmap(0, 0, shadowsCorners.at(0));
            painter.drawPixmap(pW->width() - kShadowCornerSize, 0, shadowsCorners.at(1));
            painter.drawPixmap(pW->width() - kShadowCornerSize, pW->height() - kShadowCornerSize, shadowsCorners.at(2));
            painter.drawPixmap(0, pW->height() - kShadowCornerSize, shadowsCorners.at(3));

            //edges
            painter.drawTiledPixmap(kShadowCornerSize, 0,pW->width() - kShadowCornerSize*2, scShadowSize,shadowsEdges.at(0));
            painter.drawTiledPixmap(pW->width() - scShadowSize, kShadowCornerSize,scShadowSize, pW->height() - kShadowCornerSize*2,shadowsEdges.at(1));
            painter.drawTiledPixmap(kShadowCornerSize, pW->height() - scShadowSize,pW->width() - kShadowCornerSize*2, scShadowSize,shadowsEdges.at(2));
            painter.drawTiledPixmap(0, kShadowCornerSize,scShadowSize, pW->height() - kShadowCornerSize*2,shadowsEdges.at(3));


            // rounded pixmap
            painter.setBrush(backColor);
            painter.setPen(backColor);
            painter.setOpacity(fOpacity);
            painter.drawRoundedRect(scShadowSize,scShadowSize,pW->width()-2*scShadowSize, pW->height()-2*scShadowSize,scRadiusSize,scRadiusSize);
            return true ; 
        }
        else
        {
            return QObject::eventFilter(pO,event);
        }
    }

    QWidget* notificationWidget(const Notification& pN)
    {
        NotificationType t = eNormalNotification;

        if ( pN.iconName().contains(QRegExp("notification-audio-volume-*")))
        {
            t = eProgressBarNotification ;
        }

        if ( pN.hints().contains("x-canonical-private-icon-only"))
        {
            t = eIconOnlyNotification ;
        }

        QWidget * pRet = new QWidget;
        QString summary;
        QPixmap p = pN.icon();
        switch(t)
        {
        case eIconOnlyNotification:
        case eNormalNotification:
            m_normal.setupUi(pRet);

            m_normal.iconLabel->setPixmap(p.scaled(scIconSize,scIconSize,Qt::KeepAspectRatio, Qt::SmoothTransformation));

            if( !pN.summary().isEmpty())
            {
                summary = "<b>%1</b>";
                summary = summary.arg(pN.summary());
            }

            if(!pN.body().isEmpty())
            {
                if ( !summary.isEmpty())
                {
                    summary.append("<br>");
                }
                summary.append(pN.body());
            }
            m_normal.summaryLabel->setText(summary);
            break;
        case eProgressBarNotification:
            m_progressNotification.setupUi(pRet);
            m_progressNotification.iconLabel->setPixmap(p.scaled(scIconSize,scIconSize,Qt::KeepAspectRatio, Qt::SmoothTransformation));

            m_progressNotification.progressBar->setValue(pN.hints()["value"].toInt());
            break ;
        }
        return pRet ;
    }

    // members
    QVector<QPixmap> shadowsEdges ;
    QVector<QPixmap> shadowsCorners;
    Ui::NotificationUi m_pNotificationUi;
    Ui::NormalNotification m_normal ;
    Ui::NotificationWithProgress m_progressNotification ;
    QWidget* m_pNot ;
    bool m_bShowing ;
    RazorSettings m_settings ;
    QFileSystemWatcher m_watcher ;
    quint32 m_currentNotification ;
    bool m_bEnableCompositing ;
    QScopedPointer<WidgetNotificationNotifier> m_widgetNotifier ;
};

WidgetNotification::WidgetNotification(QObject *parent):
    INotificationView(),
    d_ptr( new WidgetNotificationPrivate(this))
{
    Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::X11BypassWindowManagerHint;
    setWindowFlags(flags);

    QRect geometry = d_func()->geom() ;
    setGeometry(geometry);
    setMinimumSize(geometry.width(),geometry.height());
    setMaximumSize(geometry.width(),geometry.height());

    //set ui
    d_func()->m_pNotificationUi.setupUi(this);
    if ( d_func()->m_bEnableCompositing)
    {
        setAttribute(Qt::WA_TranslucentBackground, true);
        installEventFilter(d_func());
    }
    else
    {
//        setStyleSheet(RazorTheme::instance()->qss("notifications"));
    }

    const QString& fileName = d_func()->m_settings.fileName();

    d_func()->m_watcher.addPath(fileName);

    connect( &(d_func()->m_watcher), SIGNAL(fileChanged(QString)), SLOT(settingsChanged()));

    connect ( d_func()->m_pNotificationUi.dismissButton, SIGNAL(pressed()),this,SLOT(dismissNotification()));
}

void WidgetNotification::addNotification(const Notification&  pN)
{
    addToView(pN);
}

void WidgetNotification::removeNotification(const Notification&  pN)
{
    if ( pN.id() == d_func()->m_currentNotification )
    {
        hideNotification();
    }
}

WidgetNotification::~WidgetNotification()
{
    delete d_ptr ;
}

int WidgetNotification::viewCount() const
{
    // this plugin only shows 1 widget at the time
    return d_func()->m_bShowing == true ? 1 : 0 ;
}

void WidgetNotification::showNotification()
{
    QWidget::show();
}

void WidgetNotification::hideNotification()
{
    QWidget::hide();
    d_func()->m_pNotificationUi.dismissButton->setDown(false);
}

QObject *WidgetNotification::notifier()
{
    return d_func()->m_widgetNotifier.data();
}

void WidgetNotification::addToView(const Notification&  pN)
{

    d_func()->m_pNotificationUi.applicationNameLabel->setText(pN.appName());
    qDebug() << d_func()->m_pNotificationUi.applicationNameLabel->text();

    if ( d_func()->m_pNot != NULL )
        d_func()->m_pNotificationUi.verticalLayout_2->removeWidget(d_func()->m_pNot);
    delete d_func()->m_pNot ;
    d_func()->m_pNot=0;

    d_func()->m_pNot = d_func()->notificationWidget(pN);
    d_func()->m_pNotificationUi.verticalLayout_2->addWidget(d_func()->m_pNot);

    showNotification();

    d_func()->m_bShowing = true ;
    d_func()->m_currentNotification = pN.id();

    d_func()->m_widgetNotifier->emitNotificationShowned( pN.id() );
}

void WidgetNotification::settingsChanged()
{
    if ( isVisible() )
    {
        d_func()->m_settings.sync();
        // need to change currently visible
        // get new size
        QRect geometry = d_func()->geom() ;
        qDebug() << " settings changed new geom = " <<geometry ;
        setGeometry(geometry);
        update();
    }
}

void WidgetNotification::dismissNotification()
{
    hideNotification();
}
