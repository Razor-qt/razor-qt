#include "notification.h"
#include "notificationserversettings.h"
#include "qtnlog.h"
#include <QPixmap>
#include <QIcon>
#include <QFile>
#ifndef TEST_BUILD
#include <QDBusArgument>
#endif

#include <QApplication>
#include <QDir>


namespace
{
    static const int scMaxIconSize = 32 ;
}

Notification::Notification()
{}

Notification::Notification(const QString &appName, int id, const QString &icon, const QString &summary, const QString &body, const QStringList &actions, const QVariantMap &hints, int timeout):
    m_appName(appName), m_id ( id), m_icon( icon ), m_summary( summary ), m_body( body ), m_actions( actions ), m_hints( hints ), m_timeout( timeout)
{
}

Notification::Notification(const Notification &rhs)
{
    *this = rhs ;
}

Notification &Notification::operator =(const Notification &rhs)
{
    if ( this == &rhs)
        return *this ;
    m_appName = rhs.m_appName;
    m_id = rhs.m_id;
    m_icon = rhs.m_icon;
    m_summary = rhs.m_summary;
    m_body = rhs.m_body;
    m_actions = rhs.m_actions;
    m_hints = rhs.m_hints;
    m_timeout = rhs.m_timeout;
}

QPixmap Notification::icon() const
{

    QString icon = m_icon ;
    QPixmap pixmap;
    int maxIconSize = scMaxIconSize ;
    if(!QPixmap(icon).isNull()) //Check if icon file is available, then check if it standart icon.
    {
        pixmap = QPixmap(QIcon(icon).pixmap(maxIconSize));
    }
    else if(!m_hints["icon_data"].isNull())
    {
        pixmap = QPixmap(getPixmapFromHint(m_hints["icon_data"]));
    }
    else if(!m_hints["image_data"].isNull())
    {
        pixmap = QPixmap(getPixmapFromHint(m_hints["image_data"]));
    }
    else if(!QIcon::fromTheme(icon).pixmap(maxIconSize).isNull())
    {
        pixmap = QPixmap(QIcon::fromTheme(icon).pixmap(maxIconSize));
    }
    else
    {
        // 1 check for notifications_icon path
        QString path ;
        pixmap = QPixmap() ;

        path = NotificationServerSettings::instance()->value("notification_icons").toString();
        TRACE("Looking for icon= " <<icon.toStdString() << "in path "<< path.toStdString() );
        bool bFoundInPath = false ;

        if ( QDir().exists(path)){
            QString pathToIcon = path + "/" + icon + ".svg";
            if ( QFile::exists(pathToIcon) ) {
                pixmap.load(pathToIcon);
                if ( !pixmap.isNull() ){
                    TRACE("Icon " << pathToIcon.toStdString() << " does exists using this one");
                    bFoundInPath = true ;
                }
            }
        }

        if ( !bFoundInPath ){
            WARN("Icon was not found in path, assigning default one");
            path = qApp->applicationDirPath();
            path = path + "/NotificationUI/icon.png";
            pixmap.load(path);
            if ( pixmap.isNull() ){
                qWarning() << "Unable to get any pixmap, even default one";
            }
            else{
                TRACE( "Found default icon=" << path.toStdString() << " and it's sized " << pixmap.width() << " x " << pixmap.height());

            }
        }
    }

    return pixmap;
}


QPixmap Notification::getPixmapFromHint(const QVariant &argument) const
{
    QPixmap p;
#ifndef TEST_BUILD
    int width, height, rowstride, bitsPerSample, channels;
    bool hasAlpha;
    QByteArray data;

    const QDBusArgument arg = argument.value<QDBusArgument>();
    arg.beginStructure();
    arg >> width;
    arg >> height;
    arg >> rowstride;
    arg >> hasAlpha;
    arg >> bitsPerSample;
    arg >> channels;
    arg >> data;
    arg.endStructure();
    QImage img = QImage((uchar*)data.constData(), width, height, QImage::Format_ARGB32).rgbSwapped();

    p.convertFromImage(img);
#endif
    return p;
}
