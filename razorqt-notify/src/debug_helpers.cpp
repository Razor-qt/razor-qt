#include "qtnlog.h"
#include "notification.h"
#include <QDebug>

QDebug operator<< ( QDebug dbg, const std::string& str)
{
    dbg.nospace() << str.c_str();
    return dbg.space() ;
}

QDebug operator <<(QDebug dbg, const Notification &rhs)
{
    dbg <<"Id=" << rhs.m_id
       <<" Application Name= " << rhs.appName().toStdString()
       <<" Application summary= " << rhs.summary().toStdString()
       <<" Application body= " << rhs.body().toStdString()
       <<" timeout= " << rhs.timeout()
       <<" icon= " << rhs.m_icon.toStdString()
      <<" hints [[ ";
    QVariantMap::const_iterator it = rhs.m_hints.begin();
    QVariantMap::const_iterator itEnd = rhs.m_hints.end();
    for ( ; it != itEnd;++it)
    {
        dbg << "Name="<<it.key().toStdString() <<  "["<< it.value().typeName() << "]" ;
    }
    dbg << " ]] ";
    return dbg ;
}


