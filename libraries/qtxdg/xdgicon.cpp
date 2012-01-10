/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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

#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QCache>
#include "qiconfix/qiconloader_p.h"
#include <QtCore/QCoreApplication>

#define DEFAULT_APP_ICON "application-x-executable"

/************************************************

 ************************************************/
static void qt_cleanup_icon_cache();
typedef QCache<QString, QIcon> IconCache;
Q_GLOBAL_STATIC_WITH_INITIALIZER(IconCache, qtIconCache, qAddPostRoutine(qt_cleanup_icon_cache))

static void qt_cleanup_icon_cache()
{
    qtIconCache()->clear();
}



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
    return QIcon::themeName();
}


/************************************************
 Sets the current icon theme to name.
 ************************************************/
void XdgIcon::setThemeName(const QString& themeName)
{
    QIcon::setThemeName(themeName);
    QIconLoader::instance()->updateSystemTheme();
}


/************************************************
 Returns the QIcon corresponding to name in the current icon theme. If no such icon
 is found in the current theme fallback is return instead.
 ************************************************/
QIcon XdgIcon::fromTheme(const QString& iconName, const QIcon& fallback)
{
    if (iconName.isEmpty())
        return fallback;

    bool isAbsolute = (iconName[0] == '/');

    QString name = QFileInfo(iconName).fileName();
    if (name.endsWith(".png", Qt::CaseInsensitive) ||
        name.endsWith(".svg", Qt::CaseInsensitive) ||
        name.endsWith(".xpm", Qt::CaseInsensitive))
    {
        name.truncate(name.length() - 4);
    }

    QIcon icon;

    if (qtIconCache()->contains(name)) {
        icon = *qtIconCache()->object(name);
    } else {
        QIcon *cachedIcon;
        if (!isAbsolute)
            cachedIcon = new QIcon(new QIconLoaderEngineFixed(name));
        else
            cachedIcon = new QIcon(iconName);
        qtIconCache()->insert(name, cachedIcon);
        icon = *cachedIcon;
    }

    // Note the qapp check is to allow lazy loading of static icons
    // Supporting fallbacks will not work for this case.
    if (qApp && !isAbsolute && icon.availableSizes().isEmpty())
    {
        return fallback;
    }
    return icon;
}


/************************************************
 Returns the QIcon corresponding to names in the current icon theme. If no such icon
 is found in the current theme fallback is return instead.
 ************************************************/
QIcon XdgIcon::fromTheme(const QStringList& iconNames, const QIcon& fallback)
{
    foreach (QString iconName, iconNames)
    {
        QIcon icon = fromTheme(iconName);
        if (!icon.isNull())
            return icon;
    }

    return fallback;
}


/************************************************

 ************************************************/
QIcon XdgIcon::defaultApplicationIcon()
{
    return fromTheme(DEFAULT_APP_ICON);
}


/************************************************

 ************************************************/
QString XdgIcon::defaultApplicationIconName()
{
    return DEFAULT_APP_ICON;
}
