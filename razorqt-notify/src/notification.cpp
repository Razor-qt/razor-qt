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

Notification::Notification(const QString &appName, int id, const QString &icon, const QString &summary, const QString &body, const QStringList &actions, const QVariantMap &hints, int timeout):
    m_appName(appName), m_id ( id), m_icon( icon ), m_summary( summary ), m_body( body ), m_actions( actions ), m_hints( hints ), m_timeout( timeout)
{
}

QPixmap Notification::icon() const
{
    if ( m_pixmapCache.contains(m_icon) )
        return *(m_pixmapCache[m_icon]);

    QString icon = m_icon ;
    QPixmap *pPixmap=NULL ;
    int maxIconSize = scMaxIconSize ;
    if(!QPixmap(icon).isNull()) //Check if icon file is available, then check if it standart icon.
    {
        pPixmap = new QPixmap(QIcon(icon).pixmap(maxIconSize));
    }
    else if(!m_hints["icon_data"].isNull())
    {
        pPixmap = new QPixmap(getPixmapFromHint(m_hints["icon_data"]));
    }
    else if(!m_hints["image_data"].isNull())
    {
        pPixmap = new QPixmap(getPixmapFromHint(m_hints["image_data"]));
    }
    else if(!QIcon::fromTheme(icon).pixmap(maxIconSize).isNull())
    {
        pPixmap = new QPixmap(QIcon::fromTheme(icon).pixmap(maxIconSize));
    }
    else
    {
        // 1 check for notifications_icon path
        QString path ;
        pPixmap = new QPixmap ;

        path = NotificationServerSettings::instance()->value("notification_icons").toString();
        TRACE("Looking for icon= " <<icon.toStdString() << "in path "<< path.toStdString() );
        bool bFoundInPath = false ;

        if ( QDir().exists(path)){
            QString pathToIcon = path + "/" + icon + ".svg";
            if ( QFile::exists(pathToIcon) ) {
                pPixmap->load(pathToIcon);
                if ( !pPixmap->isNull() ){
                    TRACE("Icon " << pathToIcon.toStdString() << " does exists using this one");
                    bFoundInPath = true ;
                }
            }
        }


        if ( !bFoundInPath ){
            WARN("Icon was not found in path, assigning default one");
            path = qApp->applicationDirPath();
            path = path + "/NotificationUI/icon.png";
            pPixmap->load(path);
            if ( pPixmap->isNull() ){
                qFatal("Unable to get any pixmap, even default one");
            }
            else{
                TRACE( "Found default icon=" << path.toStdString() << " and it's sized " << pPixmap->width() << " x " << pPixmap->height());

            }
        }
    }

    m_pixmapCache.insert(icon, pPixmap );
    return *pPixmap ;
}


QPixmap Notification::getPixmapFromHint(QVariant argument) const
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
