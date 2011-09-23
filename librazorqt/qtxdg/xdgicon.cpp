/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */



#include "xdgicon.h"

#include <QString>
#include <QDebug>
#include <QDir>
#include <QStringList>
#include <QFileInfo>

#define DEFAULT_APP_ICON "application-x-executable"

/************************************************

 ************************************************/
class XdgIconCache
{
friend class XdgIcon;
public:
    QString themeName() const { return mThemeName; }
    void setThemeName(const QString& themeName);

    QIcon* const fromTheme(const QString& iconName);
    static XdgIconCache* instance();

protected:
    explicit XdgIconCache();
    virtual ~XdgIconCache();

private:
    QIcon searchFile(const QString &dir, const QString& iconName);
    QHash<QString, QIcon*> mCache;
    static XdgIconCache* mInstance;
    QString mThemeName;
};


/************************************************

 ************************************************/
XdgIcon::XdgIcon()
{
}


/************************************************

 ************************************************/
XdgIcon::~XdgIcon()
{
}


/************************************************
 Returns the name of the current icon theme.
 ************************************************/
QString XdgIcon::themeName()
{
    return XdgIconCache::instance()->themeName();
}


/************************************************
 Sets the current icon theme to name.
 ************************************************/
void XdgIcon::setThemeName(const QString& themeName)
{
    XdgIconCache::instance()->setThemeName(themeName);
}


/************************************************
 Returns the QIcon corresponding to name in the current icon theme. If no such icon
 is found in the current theme fallback is return instead.
 ************************************************/
QIcon const XdgIcon::fromTheme(const QString& iconName, const QIcon& fallback)
{
    QIcon* res = XdgIconCache::instance()->fromTheme(iconName);
    if (res)
        return *res;

    return fallback;
}


/************************************************
 Returns the QIcon corresponding to names in the current icon theme. If no such icon
 is found in the current theme fallback is return instead.
 ************************************************/
QIcon const XdgIcon::fromTheme(const QStringList& iconNames, const QIcon& fallback)
{
    foreach (QString iconName, iconNames)
    {
        QIcon* res = XdgIconCache::instance()->fromTheme(iconName);
        if (res)
            return *res;
    }

    return fallback;
}




/************************************************

 ************************************************/
XdgIconCache::XdgIconCache()
{
    mThemeName = "oxygen";
}


/************************************************

 ************************************************/
XdgIconCache::~XdgIconCache()
{

}

/************************************************

 ************************************************/
XdgIconCache* XdgIconCache::instance()
{
    if (!mInstance)
        mInstance = new XdgIconCache();
    return mInstance;
}


/************************************************

 ************************************************/
void XdgIconCache::setThemeName(const QString& themeName)
{
    mThemeName = themeName;
}


/************************************************

 ************************************************/
inline QIcon XdgIconCache::searchFile(const QString &dir, const QString& iconName)
{
    QStringList exts;
    exts << "" << ".png" << ".svg" << ".xpm";

    foreach (QString ext, exts)
    {
        QString file= QString("%1/%2%3").arg(dir, iconName, ext);
        if (QFileInfo(file).exists())
            return QIcon(file);
    }

    return QIcon();
}


/************************************************

 ************************************************/
QIcon* const XdgIconCache::fromTheme(const QString& iconName)
{
    if (iconName.isEmpty())
        return 0;

    QString key = QString("%1 %3").arg(iconName).arg(mThemeName);
    if (mCache.contains(key))
        return mCache.value(key);

    QIcon icon;

    if (iconName.startsWith(QDir::separator()))
    {
        // Absolute path
        icon = QIcon(iconName);
    }
    else
    {
        // From theme
        QIcon::setThemeName(mThemeName);
        icon = QIcon::fromTheme(iconName);

        if (icon.isNull())
            icon = QIcon::fromTheme(QFileInfo(iconName).completeBaseName());

        if (icon.isNull())
            icon = searchFile("/usr/share/pixmaps", iconName);

        if (icon.isNull())
            icon = searchFile("/usr/local/share/pixmaps", iconName);
    }


    // Some icons are drawn with the wrong size ( https://snusmumriken.nokia.kunder.linpro.no/browse/QTBUG-17953 )
    // this dirty hack fixes this. But some valid svg based icon return empty availableSizes list.
    // If you know a better solution tell me.  [ Alex Sokoloff <sokoloff.a@gmailo.com> ]
    if (!icon.isNull())
    {
        QIcon* res;
        if (icon.availableSizes().count())
        {
            res = new QIcon();
            foreach (QSize s, icon.availableSizes())
            {
                res->addPixmap(icon.pixmap(s));
            }
        }
        else
        {
            res = new QIcon(icon);
        }

        mCache[key]= res;
        return res;
    }

    //qDebug() << "XdgIcon: not found" << iconName;
    return 0;
}


/************************************************

 ************************************************/
QIcon const XdgIcon::defaultApplicationIcon()
{
    return fromTheme(DEFAULT_APP_ICON);
}
QString const XdgIcon::defaultApplicationIconName()
{
    return DEFAULT_APP_ICON;
}


XdgIconCache* XdgIconCache::mInstance=0;
