#include "qtservicesysprivate.h"

#include <QtNetwork/QTcpSocket>

QtServiceSysPrivate::QtServiceSysPrivate()
    : QtUnixServerSocket(), ident(0), serviceFlags(0)
{
}

QtServiceSysPrivate::~QtServiceSysPrivate()
{
    if (ident)
        delete[] ident;
}

void QtServiceSysPrivate::incomingConnection(int socketDescriptor)
{
    QTcpSocket *s = new QTcpSocket(this);
    s->setSocketDescriptor(socketDescriptor);
    connect(s, SIGNAL(readyRead()), this, SLOT(slotReady()));
    connect(s, SIGNAL(disconnected()), this, SLOT(slotClosed()));
}

void QtServiceSysPrivate::slotReady()
{
    QTcpSocket *s = (QTcpSocket *)sender();
    cache[s] += QString(s->readAll());
    QString cmd = getCommand(s);
    while (!cmd.isEmpty()) {
        bool retValue = false;
        if (cmd == QLatin1String("terminate")) {
            if (!(serviceFlags & QtServiceBase::CannotBeStopped)) {
                QtServiceBase::instance()->stop();
                QCoreApplication::instance()->quit();
                retValue = true;
            }
        } else if (cmd == QLatin1String("pause")) {
            if (serviceFlags & QtServiceBase::CanBeSuspended) {
                QtServiceBase::instance()->pause();
                retValue = true;
            }
        } else if (cmd == QLatin1String("resume")) {
            if (serviceFlags & QtServiceBase::CanBeSuspended) {
                QtServiceBase::instance()->resume();
                retValue = true;
            }
        } else if (cmd == QLatin1String("alive")) {
            retValue = true;
        } else if (cmd.length() > 4 && cmd.left(4) == QLatin1String("num:")) {
            cmd = cmd.mid(4);
            QtServiceBase::instance()->processCommand(cmd.toInt());
            retValue = true;
        }
        QString retString;
        if (retValue)
            retString = QLatin1String("true");
        else
            retString = QLatin1String("false");
        s->write(retString.toLatin1().constData());
        s->flush();
        cmd = getCommand(s);
    }
}

void QtServiceSysPrivate::slotClosed()
{
    QTcpSocket *s = (QTcpSocket *)sender();
    s->deleteLater();
}

QString QtServiceSysPrivate::getCommand(const QTcpSocket *socket)
{
    int pos = cache[socket].indexOf("\r\n");
    if (pos >= 0) {
        QString ret = cache[socket].left(pos);
        cache[socket].remove(0, pos+2);
        return ret;
    }
    return "";
}

