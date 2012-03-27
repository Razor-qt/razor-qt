#include "notificationserversettings.h"
#include "qtnlog.h"

#include "razorqt/razorsettings.h"

#include <QFileSystemWatcher>
#include <QDir>
#include <QFile>
#include <QPoint>
#include <QColor>
#include <QApplication>

NotificationServerSettings *NotificationServerSettings::m_pSelf = NULL ;


class NotificationServerSettingsPrivate
{
public:
    NotificationServerSettingsPrivate(NotificationServerSettings *pSettings):
        m_settings("razorqt-notify")
    {
        QString path = QDir::homePath() + "/.config/qtnotificationd";
        QDir d;
        if(!d.exists(path))
        {
            if(!d.mkpath(path))
                INFO("Making path=" << path.toStdString() << " failed");
        }

        m_settingsPath = QDir::homePath() + "/.config/qtnotificationd/config";

        if(!QFile::exists(m_settingsPath))
        {
            INFO("Settings file does not yet exists.");
            INFO("We will create default, and store it now");
            m_settingsPath = path + "/config";
            QFile f(m_settingsPath) ;
            f.open(QIODevice::ReadWrite);
            f.close();
            createDefaultSettings();
        }
    }

    void changeValue(const QString &key, const QVariant &v)
    {
        m_settings.setValue(key, v);
    }

    void createDefaultSettings()
    {
        m_settings.setValue("notification_position", QPoint(100, 100));
        m_settings.setValue("notification_size", QPoint(400, 80));
        m_settings.setValue("notification_backgroundColor", QVariant::fromValue<QColor> (QColor(3, 121, 255, 220)));

        m_settings.setValue("notification_opacity", 0.9f);
        m_settings.setValue("notification_type", "qwidget");
        QString qmlFilePathDefault = qApp->applicationDirPath() + "/NotificationUI/NotificationUI.qml";
        m_settings.setValue("QmlNotification::QMLFile", qmlFilePathDefault);
        m_settings.setValue("notification_icons", "/mnt/devel/qtnotifydaemon/images");
        m_settings.sync();

    }

    RazorSettings m_settings ;
    QString m_settingsPath ;
    QMap<QString, QVariant> m_defaults ;
};

NotificationServerSettings *NotificationServerSettings::instance(QObject *parent)
{
    if(m_pSelf == NULL) {
        m_pSelf = new NotificationServerSettings(parent);
    }

    return m_pSelf ;
}

NotificationServerSettings::NotificationServerSettings(QObject *parent) :
    QObject(parent),
    d_ptr(new NotificationServerSettingsPrivate(this))

{
    connect(&d_func()->m_settings, SIGNAL(settigsChanged()), this, SLOT(reloadSettings()));
}

NotificationServerSettings::~NotificationServerSettings()
{
    delete d_ptr ;
}

QVariant NotificationServerSettings::value(const QString &key) const
{
    QVariant ret = d_func()->m_settings.value(key);
    if(!ret.isValid()) {
        TRACE("Unable to retrive value for key=" << key.toStdString());
        TRACE("Checking for default one ");
        if(d_func()->m_defaults.contains(key)) {
            ret = d_func()->m_defaults[key];
        } else {
            WARN("Unable to find stored value neither default value for key=" << key.toStdString());
        }
    }
    return ret ;
}

void NotificationServerSettings::setValue(const QString &key, const QVariant &value)
{
    d_func()->changeValue(key, value);
    emit settingsChanged();
}

void NotificationServerSettings::addKey(const QString &key, const QVariant &defaultValue)
{
    if(d_func()->m_defaults.contains(key)) {
        WARN("Default value for key=" << key.toStdString() << " already exists");
    } else
        d_func()->m_defaults[key] = defaultValue ;

    // FIX: does not set actual value, now do it on
    d_func()->m_settings.setValue(key, defaultValue);
    d_func()->m_settings.sync();
}

void NotificationServerSettings::addKeyWithAllowedValues(const QString &key, const QVariantList &listOfValues)
{
    //TODO: to implement
    Q_UNUSED(key)
    Q_UNUSED(listOfValues)
}

void NotificationServerSettings::reloadSettings()
{
    INFO("NotificationServerSettings::reloadSettings()");
    // verification of values?
    emit settingsChanged();
}


