#ifndef QTNLOG_H
#define QTNLOG_H

#include <QDebug>

class Notification ; 

#define TRACE(Message) qDebug() << "TRACE "<< Message ;
#define INFO(Message)  qDebug() << "INFO " << Message ;
#define WARN(Message)  qWarning() << "WARN " << Message ;
#define FATAL(Message) qFatal() << "FATAL" << Message ;
#define CONTEXT_PUSH(Message) INFO("Starting context " << Message)
#define CONTEXT_POP INFO("Context finished")

QDebug operator<< ( QDebug dbg, const std::string& str);
QDebug operator<< ( QDebug dbg, const Notification& str);

#endif // QTNLOG_H
