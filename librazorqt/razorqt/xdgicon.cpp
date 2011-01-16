/********************************************************************
  Copyright: 2010 Alexander Sokoloff <sokoloff.a@gmail.ru>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*********************************************************************/

#include "xdgicon.h"
#include <QString>
#include <QDebug>
#include <QDir>
#include <QStringList>
#include <QFileInfo>

/************************************************

 ************************************************/
class XdgIconCache
{
friend class XdgIcon;
public:
    QString themeName() const { return mThemeName; }
    void setThemeName(const QString& themeName);

    QIcon* const fromTheme(const QString& iconName, int size);
    static XdgIconCache* instance();

protected:
    explicit XdgIconCache();
    virtual ~XdgIconCache();

private:
    QIcon searchFile(const QString& fileName);
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

 ************************************************/
QString XdgIcon::themeName()
{
    return XdgIconCache::instance()->themeName();
}


/************************************************

 ************************************************/
void XdgIcon::setThemeName(const QString& themeName)
{
    XdgIconCache::instance()->setThemeName(themeName);
}


/************************************************

 ************************************************/
QIcon const XdgIcon::fromTheme(const QString& iconName, int size, const QIcon& fallback)
{
    QIcon* res = XdgIconCache::instance()->fromTheme(iconName, size);
    if (res)
        return *res;

    return fallback;
}


/************************************************

 ************************************************/
QIcon const XdgIcon::fromTheme(const QStringList& iconNames, int size, const QIcon& fallback)
{
    foreach (QString iconName, iconNames)
    {
        QIcon* res = XdgIconCache::instance()->fromTheme(iconName, size);
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
inline QIcon XdgIconCache::searchFile(const QString& fileName)
{
    QFileInfo file(fileName);
    if (file.exists())
        return QIcon(fileName);

    return QIcon();
}


/************************************************

 ************************************************/
QIcon* const XdgIconCache::fromTheme(const QString& iconName, int size)
{
    QString key = QString("%1 %2 %3").arg(iconName).arg(size).arg(mThemeName);
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
           icon = searchFile(QString("/usr/share/pixmaps/%1").arg(iconName));

        if (icon.isNull())
           icon = searchFile(QString("/usr/share/pixmaps/%1.png").arg(iconName));

        if (icon.isNull())
           icon = searchFile(QString("/usr/share/pixmaps/%1.svg").arg(iconName));

        if (icon.isNull())
           icon = searchFile(QString("/usr/share/pixmaps/%1.xpm").arg(iconName));

    }


    if (!icon.isNull())
    {
        QIcon* res = new QIcon(icon.pixmap(QSize(size, size)));
        mCache[key]= res;
        return res;
    }

    //qDebug() << "XdgIcon: not found" << iconName;
    return 0;
}


XdgIconCache* XdgIconCache::mInstance=0;
