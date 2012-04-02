#ifndef QTSERVICESTARTER_H
#define QTSERVICESTARTER_H

#include <QObject>
#include "qtservice_p.h"

class QtServiceBasePrivate ;

class QtServiceStarter : public QObject
{
    Q_OBJECT
public:
    QtServiceStarter(QtServiceBasePrivate *service)
        : QObject(), d_ptr(service) {}
public slots:
    void slotStart()
    {
        d_ptr->startService();
    }
private:
    QtServiceBasePrivate *d_ptr;
};

#endif // QTSERVICESTARTER_H
