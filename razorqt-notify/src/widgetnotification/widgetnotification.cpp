#include "widgetnotification.h"
#include "notification.h"
#include "qtnlog.h"
#include "notificationserversettings.h"

#include "ui_notification.h"

#include <QStackedWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>

#include <QPropertyAnimation>

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
    WidgetNotificationPrivate( WidgetNotification* pParent ):
        m_pNotificationWidget(NULL),
        m_pNotificationUi(NULL),
        m_bShowing(true)
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
        QPoint p = NotificationServerSettings::instance()->value("notification_position").toPoint();
        QPoint size = NotificationServerSettings::instance()->value("notification_size").toPoint();
        int startX = p.x() ;
        int startY = p.y() ;
        return QRect(startX,startY,size.x(),size.y());
    }

    bool eventFilter(QObject *pO, QEvent *event)
    {
        if ( event->type() == QEvent::Paint)
        {
            QWidget* pW = qobject_cast<QWidget*>(pO);

            QColor backColor = NotificationServerSettings::instance()->value("notification_backgroundColor").value<QColor>();
            float fOpacity = NotificationServerSettings::instance()->value("notification_opacity").value<float>();
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


    // members
    QVector<QPixmap> shadowsEdges ;
    QVector<QPixmap> shadowsCorners ;

    QWidget* m_pNotificationWidget ;
    Ui::NotificationUi* m_pNotificationUi;

    bool m_bShowing ;

};

WidgetNotification::WidgetNotification(QObject *parent):
    INotificationView(parent),
    d_ptr( new WidgetNotificationPrivate(this))
{
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

void WidgetNotification::show()
{

    if ( d_func()->m_pNotificationWidget != NULL )
    {
        d_func()->m_pNotificationWidget->show();
    }
}

void WidgetNotification::hide()
{

    if ( d_func()->m_pNotificationWidget != NULL )
        d_func()->m_pNotificationWidget->hide();
}

void WidgetNotification::addToView(Notification *pN)
{

    if ( NULL == d_func()->m_pNotificationWidget && NULL == d_func()->m_pNotificationUi )
    {
        d_func()->m_pNotificationWidget = new QWidget ;
        d_func()->m_pNotificationWidget->installEventFilter(d_func());
        d_func()->m_pNotificationUi = new Ui::NotificationUi ;

        // set widget
        Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::X11BypassWindowManagerHint;

        d_func()->m_pNotificationWidget->setWindowFlags(flags);

        QRect geometry = d_func()->geom() ;
        d_func()->m_pNotificationWidget->setGeometry(geometry);
        d_func()->m_pNotificationWidget->setMinimumSize(geometry.width(),geometry.height());
        d_func()->m_pNotificationWidget->setMaximumSize(geometry.width(),geometry.height());

        //set ui
        d_func()->m_pNotificationUi->setupUi(d_func()->m_pNotificationWidget);

        d_func()->m_pNotificationWidget->setAttribute(Qt::WA_TranslucentBackground, true);


        //d_func()->m_pNotificationWidget->show();
        show();
    }

    QPixmap p = pN->icon();
    d_func()->m_pNotificationUi->iconLabel->setPixmap(p.scaled(scIconSize-10,scIconSize-10,Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QString summ = pN->summary();
    d_func()->m_pNotificationUi->applicationSummaryLabel->setText(summ);
    d_func()->m_pNotificationUi->applicationBodyLabel->setText(pN->body());
    if ( !d_func()->m_pNotificationWidget->isVisible())
//        d_func()->m_pNotificationWidget->show();
        show();

    d_func()->m_bShowing = true ;
}

void WidgetNotification::remove(Notification *pN)
{
    d_func()->m_bShowing = false ;
    requireRemove(pN->id());
}
