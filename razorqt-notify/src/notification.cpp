#include "notification.h"
#include "qtnlog.h"
#include <QPixmap>
#include <QIcon>
#include <QFile>
#include <QDBusArgument>
#include <QApplication>
#include <QDir>

#include "razorsettings.h"
#include "xdgicon.h"



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

    return *this ;
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

        // FIXME: don't create settings every time
        RazorSettings s ("razorqt-notify");

        path = s.value("notification_icons").toString();
        bool bFoundInPath = false ;

        if ( QDir().exists(path)){
            QString pathToIcon = path + "/" + icon + ".svg";
            if ( QFile::exists(pathToIcon) ) {
                pixmap.load(pathToIcon);
                if ( !pixmap.isNull() ){
                    bFoundInPath = true ;
                }
            }
        }
        if ( pixmap.isNull() && !icon.isEmpty())
        {
            QString iconEmbedded = ":/";
            iconEmbedded.append(icon);
            iconEmbedded.append(".svg");
            qDebug() << " Searching embedded icon=" << iconEmbedded;
            pixmap = QPixmap(iconEmbedded);
            if ( !pixmap.isNull())
            {
                bFoundInPath = true ;
            }
        }
        // TODO: if icons not found, try to search for application icon ??
        // FIXME: for now, icons are embedded in this executable

        if ( !bFoundInPath )
        {
            qDebug() << "Icon was not found in path, assigning default one";
            pixmap = QPixmap (XdgIcon::defaultApplicationIcon().pixmap(QSize(64,64)));
            if ( pixmap.isNull() )
            {
                qWarning() << "Unable to get any pixmap, even default one";
            }
            else
            {
                qDebug() <<  "Found default icon=" << path.toStdString() << " and it's sized " << pixmap.width() << " x " << pixmap.height();
            }
        }
    }
    return pixmap;
}

const QString &Notification::summary() const
{

    if ( m_summary.isEmpty())
    {

    }
    return m_summary;
}


QPixmap Notification::getPixmapFromHint(const QVariant &argument) const
{
    QPixmap p;
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
    return p;
}
