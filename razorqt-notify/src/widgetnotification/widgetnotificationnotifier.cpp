#include "widgetnotificationnotifier.h"

WidgetNotificationNotifier::WidgetNotificationNotifier(QObject *parent) :
    QObject(parent)
{
}

void WidgetNotificationNotifier::emitNotificationShowned(quint32 notificationId)
{
    emit notificationShowned(notificationId);
}

