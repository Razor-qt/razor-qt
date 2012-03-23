#ifndef NOTIFICATIONSERVERSETTINGS_H
#define NOTIFICATIONSERVERSETTINGS_H

#include <QObject>
#include <QSettings>

class NotificationServerSettingsPrivate;

class NotificationServerSettings : public QObject
{
    Q_OBJECT
public:

    static NotificationServerSettings* instance(QObject *parent=NULL) ;

    ~NotificationServerSettings();

    /**
     * @brief Returns value for key. If value does not exists, return default value
     * @param key
     */
    QVariant value(const QString &key) const ;
    /**
     * @brief
     *
     * @param key
     * @param value
     */
    void setValue( const QString& key, const QVariant& value);

    /**
     * @brief
     * @param key
     * @param defaultValue
     */
    void addKey( const QString& key, const QVariant& defaultValue);

    void addKeyWithAllowedValues(const QString& key, const QVariantList& listOfValues);
    
signals:
    void settingsChanged() ;
public slots:
    void reloadSettings() ;
private:
    Q_DECLARE_PRIVATE(NotificationServerSettings)
    NotificationServerSettingsPrivate* const d_ptr ;
    static NotificationServerSettings* m_pSelf ;
private:
    explicit NotificationServerSettings(QObject *parent = 0);


};

#endif // NOTIFICATIONSERVERSETTINGS_H
