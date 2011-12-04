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


#include "xdgmime.h"
#include "xdgicon.h"

#include <QFileInfo>
#include <magic.h>
#include <QDebug>
#include <QtCore/QStringList>


/************************************************

 ************************************************/
XdgMimeInfo::XdgMimeInfo(const QString& mimeType)
{
    mType = mimeType.section('/', 0, 0);
    mSubType = mimeType.section('/', 1);
}


/************************************************

 ************************************************/
QString getFileMimeType(const QFileInfo& fileInfo)
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
XdgMimeInfo::XdgMimeInfo(const QFileInfo& file)
{
    QString mimeType = getFileMimeType(file);
    mType = mimeType.section('/', 0, 0);
    mSubType = mimeType.section('/', 1);
}


/************************************************

 ************************************************/
QString XdgMimeInfo::mimeType() const
{
    return mType + "/" + mSubType;
}


/************************************************

 ************************************************/
QString XdgMimeInfo::iconName() const
{
    QStringList names;
    names << QString("%1-x-%2").arg(mType, mSubType);
    names << QString("%1-%2").arg(mType, mSubType);
    names << QString("%1-x-generic").arg(mType);
    names << QString("%1-generic").arg(mType);

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

