#ifndef QTNDBUSCONNECTOR_H
#define QTNDBUSCONNECTOR_H

#include <QDBusAbstractAdaptor>
#include <QStringList>

class QApplication;
class NotificationHandler;
class QtnDbusConnectorPrivate;

/**
 * @brief D-bus connector, to connect with org.freedesktop.Notifications
 **/
class QtnDbusConnector : public QDBusAbstractAdaptor
{
    Q_DECLARE_PRIVATE(QtnDbusConnector)
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Notifications")
public:
    explicit QtnDbusConnector(QApplication* pA );
    void connectToDbus();

signals:
    Q_INVOKABLE void NotificationClosed(unsigned id, unsigned reason);
    Q_INVOKABLE void ActionInvoked(unsigned int, QString);

public slots: /* Dbus invokable */
    Q_INVOKABLE QStringList GetCapabilities();
    Q_INVOKABLE QString GetServerInformation(QString& vendor, QString& version, QString& spec_version);
    Q_INVOKABLE quint32 Notify(QString app_name, unsigned id, QString icon, QString summary, QString body, QStringList actions, QVariantMap hints, int timeout);
    Q_INVOKABLE void CloseNotification(quint32 id);

    Q_INVOKABLE void show() ;
    Q_INVOKABLE void hide() ;

private: /*members*/
    QtnDbusConnectorPrivate* const d_ptr ;

};

#endif // QTNDBUSCONNECTOR_H
