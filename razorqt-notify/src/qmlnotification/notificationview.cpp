#include "notificationview.h"
#include "notification.h"
#include "notificationtimeout.h"

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


QmlNotificationView::QmlNotificationView(QWidget* parent /*= NULL */)
{
    setSource(QUrl("qrc:/notification.qml"));
}

void QmlNotificationView::addNotification(const Notification &pN)
{
}

void QmlNotificationView::removeNotification(const Notification &pN)
{
}

QObject *QmlNotificationView::notifier()
{
    return NULL ;
}

int QmlNotificationView::viewCount() const
{
    return 1 ;
}

void QmlNotificationView::showNotification()
{
    show();
}

void QmlNotificationView::hideNotification()
{
    hide();
}
