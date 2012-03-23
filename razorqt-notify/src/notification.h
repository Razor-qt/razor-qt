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
    Notification(const QString& appName, int id, const QString& icon, const QString& summary, const QString& body, const QStringList& actions, const QVariantMap& hints, int timeout);
    QPixmap icon() const ;

    int id() const { return m_id ; }
    const QString& appName() const { return m_appName ; }
    const QString& summary() const { return m_summary ; }
    const QString& body() const { return m_body ; }
    int timeout() const { return m_timeout ; }

private:

    QString m_appName ;
    int m_id ;
    QString m_icon ;
    QString m_summary ;
    QString m_body ;
    QStringList m_actions ;
    QVariantMap m_hints ;
    int m_timeout ;
    mutable QCache<QString, QPixmap> m_pixmapCache ;

    QPixmap getPixmapFromHint(QVariant argument) const ;

    friend QDebug operator<< ( QDebug dbg, const Notification& str);

};

#endif // NOTIFICATION_H
