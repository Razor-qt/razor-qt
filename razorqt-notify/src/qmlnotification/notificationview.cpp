#include "notificationview.h"
#include "notification.h"
#include "notificationtimeout.h"
#include "notificationserversettings.h"

#include <QtDeclarative/QDeclarativeView>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QSignalMapper>
#include <QApplication>
#include <QGraphicsObject>
#include <QDesktopWidget>
#include <QFile>

#include "qtnlog.h"

namespace {
    //static QString scPath;
}

NotificationView::NotificationView(QObject* parent) :
    INotificationView(parent),
    m_pMainWidget( new QWidget),
    m_pQmlView( new QDeclarativeView())
{
    m_pMainWidget->setWindowFlags(Qt::ToolTip);
    m_pMainWidget->setAttribute(Qt::WA_TranslucentBackground);
    m_pMainWidget->setStyleSheet("background:transparent;");
    QLayout* pL = new QHBoxLayout(m_pMainWidget);
    pL->setMargin(0);
    pL->addWidget(m_pQmlView);

    QString qmlPath = NotificationServerSettings::instance()->value("QmlNotification::QMLFile").toString();
    TRACE("Default qml file is qmlPath=" << qmlPath.toStdString());
    m_pQmlView->setSource(QUrl(qmlPath));

    QObject* pO = m_pQmlView->rootObject();

    connect( pO, SIGNAL(qmlRemoved(int)), this, SLOT(removedFromQml(int)));
}

NotificationView::~NotificationView()
{
    delete m_pMainWidget ;
}

void NotificationView::show()
{
    bool bShow = false ;

    if ( !m_pMainWidget->isVisible() )
        bShow = true ;

    // is during animation?
    if ( bShow )
        m_pMainWidget->show();
}

void NotificationView::addToView(Notification *pN)
{

    QObject* pRoot = qobject_cast<QObject*> (m_pQmlView->rootObject()); // cast
    QVariant returnedValue ;
    QString pixmapPath = qApp->applicationDirPath() +"/tmp%1.png";
    int randomValue = qrand();
    pixmapPath = pixmapPath.arg(randomValue);

    QPixmap p = pN->icon() ;
    p = p.scaled(60,60,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    p.save(pixmapPath);

    QMetaObject::invokeMethod(
                pRoot,
                "addSomeData",
                Q_RETURN_ARG(QVariant, returnedValue),
                Q_ARG(QVariant, pN->appName()),
                Q_ARG(QVariant, pN->summary()),
                Q_ARG(QVariant, pixmapPath),
                Q_ARG(QVariant, pN->body()),
                Q_ARG(QVariant, pN->id())
                );

}

void NotificationView::hide()
{
    m_pMainWidget->hide();
}

void NotificationView::removedFromQml(int index)
{
    INFO("Someone clicked remove from qml index = " << index );
    INotificationView::requireRemove(index);
}

void NotificationView::qmlRemove(Notification *pN)
{
    QVariant returnedValue ;
    QString pixmapPath ;
    QObject* pRoot = qobject_cast<QObject*> (m_pQmlView->rootObject()); // cast
    if ( NULL == pN ) {
        WARN("NotificationView::qmlRemove pointer is NULL, return ");
        return ;
    }
    QMetaObject::invokeMethod(
                pRoot,
                "removeItem",
                Q_RETURN_ARG(QVariant, returnedValue),
                Q_ARG(QVariant, pN->appName()),
                Q_ARG(QVariant, pN->summary()),
                Q_ARG(QVariant, pixmapPath),
                Q_ARG(QVariant, pN->body()),
                Q_ARG(QVariant, pN->id())
                );
}

int NotificationView::qmlCount() const
{
    int intRet =-1 ;
    QVariant returnedValue ;
    QObject* pRoot = qobject_cast<QObject*> (m_pQmlView->rootObject()); // cast
    QMetaObject::invokeMethod(pRoot, "count",Q_RETURN_ARG(QVariant, returnedValue));
    bool bOk ;
    intRet = returnedValue.toInt(&bOk);
    if ( !bOk )
        WARN("Unable to cast returned value from count function invoke");
    return intRet ;
}

void NotificationView::remove(Notification *pN)
{
    qmlRemove(pN);
//    int c = qmlCount();
//    if ( c == 0 )
//         hide();
}

int NotificationView::viewCount() const
{
    return qmlCount() ;
}
