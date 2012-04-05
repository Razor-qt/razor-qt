#ifndef QTSERVICESYSPRIVATE_H
#define QTSERVICESYSPRIVATE_H

#include <QObject>
#include "qtunixserversocket.h"
#include "QtServiceBase"
#include <QMap>

class QtServiceSysPrivate : public QtUnixServerSocket
{
    Q_OBJECT
public:
    QtServiceSysPrivate();
    ~QtServiceSysPrivate();

    char *ident;

    QtServiceBase::ServiceFlags serviceFlags;

protected:
    void incomingConnection(int socketDescriptor);

private slots:
    void slotReady();
    void slotClosed();

private:
    QString getCommand(const QTcpSocket *socket);
    QMap<const QTcpSocket *, QString> cache;
};

#endif // QTSERVICESYSPRIVATE_H
