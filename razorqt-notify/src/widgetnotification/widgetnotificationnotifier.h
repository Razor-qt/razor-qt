#ifndef WIDGETNOTIFICATIONNOTIFIER_H
#define WIDGETNOTIFICATIONNOTIFIER_H

#include <QObject>

class WidgetNotificationNotifier : public QObject
{
    Q_OBJECT
public:
    explicit WidgetNotificationNotifier(QObject *parent = 0);
    void emitNotificationShowned(quint32 notificationId);
    
signals:
    void notificationShowned(quint32);
public slots:
    
};

#endif // WIDGETNOTIFICATIONNOTIFIER_H
