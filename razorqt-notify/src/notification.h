#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QObject>
#include <QVariantMap>
#include <QStringList>
#include <QCache>
#include <QPixmap>

class Notification
{

public:
    Notification();
    Notification(const QString& appName, int id, const QString& icon, const QString& summary, const QString& body, const QStringList& actions, const QVariantMap& hints, int timeout);
    Notification( const Notification& rhs );
    Notification& operator=(const Notification& rhs);


    QPixmap icon() const ;
    const QString& iconName() const { return m_icon;}

    quint32 id() const { return m_id ; }
    const QString& appName() const { return m_appName ; }
    const QString& summary() const ;
    const QString& body() const { return m_body ; }
    qint32 timeout() const { return m_timeout ; }
    void setTimeout( qint32 timeout) { m_timeout = timeout ; }
    const QVariantMap& hints() const { return m_hints;}

private:

    QString m_appName ;
    quint32 m_id ;
    QString m_icon ;
    QString m_summary ;
    QString m_body ;
    QStringList m_actions ;
    QVariantMap m_hints ;
    qint32 m_timeout ;

    QPixmap getPixmapFromHint(const QVariant &argument) const ;

    friend QDebug operator<< ( QDebug dbg, const Notification& str);

};

#endif // NOTIFICATION_H
