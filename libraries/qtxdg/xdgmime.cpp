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


#include "xdgmime.h"
#include "xdgicon.h"
#include "xdgdirs.h"

#include <QFileInfo>
#include <magic.h>
#include <QDebug>
#include <QtCore/QStringList>
#include <QMap>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QSharedData>


struct XdgMimeData
{
public:
    XdgMimeData(QString media, QString subtype);
    bool readXml(QIODevice* xml);
    
    QString mMedia;
    QString mSubtype;

    bool mDbLoaded;
    QString mComment;
    QMap<QString, QString> mLocalizedComments;
    QStringList mPatterns;
    QString mSubClassOf;
};



/************************************************

 ************************************************/
XdgMimeInfo::XdgMimeInfo(const QString& mimeType)
{
    QString media = mimeType.section('/', 0, 0);
    QString subtype = mimeType.section('/', 1);
    mData = new XdgMimeData(media, subtype);
}


/************************************************

 ************************************************/
QString getFileMimeType(const QFileInfo& fileInfo, bool followSymLinks)
{

    QString result("application/octet-stream");

    magic_t magicMimePredictor;
    magicMimePredictor = magic_open(MAGIC_MIME_TYPE); // Open predictor
    if (!magicMimePredictor) {
        qWarning() << "libmagic: Unable to initialize magic library";
        return result;
    }


    if (magic_load(magicMimePredictor, 0)) { // if not 0 - error
        qWarning() << QString("libmagic: Can't load magic database - %1").arg(magic_error(magicMimePredictor));
        magic_close(magicMimePredictor); // Close predictor
        return result;
    }

    QByteArray ar = fileInfo.absoluteFilePath().toLocal8Bit();
    if (followSymLinks && fileInfo.isSymLink())
    {
        ar = fileInfo.symLinkTarget().toLocal8Bit();
    }
    char *file = ar.data();

    // getting mime-type ........................
    const char *mime;
    mime = magic_file(magicMimePredictor, file);
    result = QString(mime);

    // Close predictor ..........................
    magic_close(magicMimePredictor);

    return result;
}


/************************************************

 ************************************************/
XdgMimeInfo::XdgMimeInfo(const QFileInfo& file, bool followSymLinks)
{
    QString mimeType = getFileMimeType(file, followSymLinks);
    QString media = mimeType.section('/', 0, 0);
    QString subtype = mimeType.section('/', 1);
    mData = new XdgMimeData(media, subtype);
}


/************************************************

 ************************************************/
QString XdgMimeInfo::mimeType() const
{
    return mData->mMedia + "/" + mData->mSubtype;
}

QString XdgMimeInfo::comment() const
{
    return mData->mComment;
}

QString XdgMimeInfo::localizedComment() const
{
    // FIXME
    return mData->mComment;
}

QStringList XdgMimeInfo::patterns() const
{
    return mData->mPatterns;
}

/************************************************

 ************************************************/
QString XdgMimeInfo::iconName() const
{
    QStringList names;
    names << QString("%1-x-%2").arg(mData->mMedia, mData->mSubtype);
    names << QString("%1-%2").arg(mData->mMedia, mData->mSubtype);
    names << QString("%1-x-generic").arg(mData->mMedia);
    names << QString("%1-generic").arg(mData->mMedia);

    foreach (QString s, names)
    {
        if (!XdgIcon::fromTheme(s).isNull())
            return s;
    }

    return "unknown";
}

/************************************************

 ************************************************/
QIcon XdgMimeInfo::icon() const
{
    return XdgIcon::fromTheme(iconName());
}


QString XdgMimeInfo::subClassOf() const
{
    return mData->mSubClassOf;
}


bool XdgMimeInfo::loadFromDb(QIODevice* xml)
{
    return mData->readXml(xml);
}



XdgMimeData::XdgMimeData(QString media, QString subtype): 
        mMedia(media),
        mSubtype(subtype),
        mDbLoaded(false)
{
}

bool XdgMimeData::readXml(QIODevice* xml)
{
    QDomDocument domDocument;
    if (! domDocument.setContent(xml, false))
    {
        return false;
    }

    QDomElement rootElement = domDocument.documentElement();
    if (rootElement.nodeName() != "mime-type")
    {
        return false;
    }

    if (rootElement.attribute("type") != mMedia + "/" + mSubtype)
    {
        return false;
    }

    QDomNodeList commentNodes = rootElement.elementsByTagName("comment");
    for(uint i = 0; i < commentNodes.length(); i++)
    {
        if (! commentNodes.item(i).isElement())
        {
            continue;
        }

        QDomElement commentElement = commentNodes.item(i).toElement();

        if (commentElement.hasAttribute("xml:lang"))
        {
            mLocalizedComments[commentElement.attribute("xml:lang")] = commentElement.text();
        }
        else 
        {
            mComment = commentElement.text();
        }
    }

    QSet<QString> collectedPatterns; 
    QDomNodeList globNodes = rootElement.elementsByTagName("glob");
    for(uint i = 0; i < globNodes.length(); i++)
    {
        if (globNodes.item(i).isElement() && globNodes.item(i).toElement().hasAttribute("pattern"))
        {
            collectedPatterns << globNodes.item(i).toElement().attribute("pattern");
        }
    }
    
    mPatterns = collectedPatterns.toList();
    mPatterns.sort();

    QDomNodeList subClassOfElements = rootElement.elementsByTagName("sub-class-of");
    if (subClassOfElements.size() > 0)
    {
        mSubClassOf = subClassOfElements.at(0).toElement().attribute("type");
    }

    return true;
}


QStringList XdgMimeInfoCache::mediatypes()
{
    return cache().keys();
}

QStringList XdgMimeInfoCache::subtypes(const QString& media)
{
    return cache().value(media).keys();
}

XdgMimeInfo* XdgMimeInfoCache::xdgMimeInfo(const QString & media, const QString & subtype)
{
    return cache().value(media).value(subtype);
}

XdgMimeInfo* XdgMimeInfoCache::xdgMimeInfo(const QString& mimetype)
{
    QString media = mimetype.section("/", 0, 0);
    QString subtype = mimetype.section("/", 1, 1);
    return xdgMimeInfo(media, subtype);
}


void loadMimeInfoCache(QMap<QString, QMap<QString, XdgMimeInfo*> > & cache)
{
    qDebug() << "loadMimeInfoCache";
    QStringList datadirs = XdgDirs::dataDirs(); 
    datadirs.prepend(XdgDirs::dataHome(false));
    const QStringList filters = (QStringList() << "*.xml");
 
    foreach (const QString datadir, datadirs)
    {
        QDir mimedir(datadir + "/mime");
        
        if (! mimedir.exists())
        {
            continue;
        }
        
       
        foreach (QFileInfo mediadirInfo, mimedir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
        {
            QString media = mediadirInfo.fileName();
            
            QDir mediadir(mediadirInfo.absoluteFilePath()); 
            foreach (QFileInfo subtypefileInfo, mediadir.entryInfoList(filters, QDir::Files))
            {
                QString subtype = subtypefileInfo.fileName().left(subtypefileInfo.fileName().length() - 4);
                //qDebug() << "subtype:" << subtype;
                QFile subtypefile(subtypefileInfo.absoluteFilePath());
                XdgMimeInfo* mimeInfo = new XdgMimeInfo(media + "/" + subtype);
                if (subtypefile.open(QIODevice::ReadOnly) && mimeInfo->loadFromDb(&subtypefile))
                {
                    cache[media][subtype] = mimeInfo;
                }
                else 
                {
                    delete mimeInfo;
                }
                
            }
        }
    }

    // TESTING
    XdgMimeData data("application", "msword");
    QFile mswordxml("/usr/share/mime/application/msword.xml");
    mswordxml.open(QIODevice::ReadOnly);
    data.readXml(&mswordxml);
    qDebug() << "=================================================================================";
    qDebug() << "data:" << data.mMedia << data.mSubtype << data.mComment << data.mLocalizedComments << data.mPatterns;
    qDebug() << "=================================================================================";
}

QMap<QString, QMap<QString, XdgMimeInfo*> > & XdgMimeInfoCache::cache()
{
    static QMap<QString, QMap<QString, XdgMimeInfo*> > _cache;
    static bool cache_loaded = false;

    if (! cache_loaded)
    {
        loadMimeInfoCache(_cache);
        cache_loaded = true;
    }
    
    return _cache;
}