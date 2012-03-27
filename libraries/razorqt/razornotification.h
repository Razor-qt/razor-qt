#ifndef RAZORNOTIFICATION_H
#define RAZORNOTIFICATION_H

#include <QtGlobal>
#include <QScopedPointer>
#include <QVariantMap>

class QString ;
class RazorNotificationPrivate;

/**
 * @brief Single razor notification. Using org.freedesktop.Notifications
 */
class RazorNotification
{
public:
    RazorNotification();

    /**
     * @brief A static method provided for convenience. Creates RazorNotification object and call Notify method
     * @see Notify
     * @note for specification please look @ http://www.galago-project.org/specs/notification/0.9/x408.html#command-notify
     *
     * @param appName Your application name
     * @param replace_id id of notification you want to replace. 0 is not interested in replacing notifications
     * @param appIcon icon name.
     * @param summary
     * @param body
     * @param hints see http://www.galago-project.org/specs/notification/0.9/x344.html
     * @param expire_timeout
     * @return id of notification
     */
    static quint32 notify(const QString& appName,quint32 replace_id,const QString& appIcon, const QString& summary, const QString& body,
                       const QVariantMap& hints, qint32 expire_timeout);

    static void closeNotification(quint32 id);
private:
    Q_DECLARE_PRIVATE(RazorNotification)
    QScopedPointer<RazorNotificationPrivate> const d_ptr;
};

#endif // RAZORNOTIFICATION_H
