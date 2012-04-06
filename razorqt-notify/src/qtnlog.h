#ifndef QTNLOG_H
#define QTNLOG_H

#include <QDebug>

class Notification ;


QDebug operator<< ( QDebug dbg, const std::string& str);
QDebug operator<< ( QDebug dbg, const Notification& str);

#endif // QTNLOG_H
