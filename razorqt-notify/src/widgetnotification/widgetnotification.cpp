#include "widgetnotification.h"
#include "notification.h"
#include "qtnlog.h"

#include "ui_notification.h"
#include "razorsettings.h"
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QFileSystemWatcher>

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
        m_pNotificationUi(NULL),
        m_bShowing(true),
        m_settings("razorqt-notify"),
        m_currentNotification(0)
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


    // members
    QVector<QPixmap> shadowsEdges ;
    QVector<QPixmap> shadowsCorners;
    Ui::NotificationUi* m_pNotificationUi;
    bool m_bShowing ;
    RazorSettings m_settings ;
    QFileSystemWatcher m_watcher ;
    quint32 m_currentNotification ;
};

WidgetNotification::WidgetNotification(QObject *parent):
    INotificationView(),
    d_ptr( new WidgetNotificationPrivate(this))
{
    installEventFilter(d_func());
    d_func()->m_pNotificationUi = new Ui::NotificationUi ;

    Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::X11BypassWindowManagerHint;

    setWindowFlags(flags);

    QRect geometry = d_func()->geom() ;
    setGeometry(geometry);
    setMinimumSize(geometry.width(),geometry.height());
    setMaximumSize(geometry.width(),geometry.height());

    //set ui
    d_func()->m_pNotificationUi->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground, true);

    // connect to settings changed
    qDebug() << " Connecting to settings";

    const QString& fileName = d_func()->m_settings.fileName();

    d_func()->m_watcher.addPath(fileName);

    connect( &(d_func()->m_watcher), SIGNAL(fileChanged(QString)), SLOT(settingsChanged()));

    connect ( d_func()->m_pNotificationUi->dismissButton, SIGNAL(pressed()),this,SLOT(dismissNotification()));
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
    d_func()->m_pNotificationUi->dismissButton->setDown(false);
}

void WidgetNotification::addToView(const Notification&  pN)
{
    QPixmap p = pN.icon();
    d_func()->m_pNotificationUi->iconLabel->setPixmap(p.scaled(scIconSize,scIconSize,Qt::KeepAspectRatio, Qt::SmoothTransformation));
    d_func()->m_pNotificationUi->applicationNameLabel->setText(pN.appName());
    if( !pN.summary().isEmpty())
    {
        d_func()->m_pNotificationUi->applicationBodyLabel->setText(pN.summary());
    }
    else if(!pN.body().isEmpty())
    {
        d_func()->m_pNotificationUi->applicationBodyLabel->setText(pN.body());
    }
    showNotification();

    d_func()->m_bShowing = true ;
    d_func()->m_currentNotification = pN.id();
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
