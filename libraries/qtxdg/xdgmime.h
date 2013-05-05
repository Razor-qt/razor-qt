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


#ifndef QTXDG_XDGMIME_H
#define QTXDG_XDGMIME_H

#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtGui/QIcon>

struct XdgMimeData;

/*! @brief The XdgMimeInfo class provides mime information about file.
 */
class XdgMimeInfo
{
public:
    /// Constructs a RazorMimeInfo with the mimeType type.
    explicit XdgMimeInfo(const QString& mimeType);

    /**
    Constructs a new RazorMimeInfo that gives mime information about the given file.
    If file is symlink and followSymLinks is true function gives information for the
    file the link references rather than for the link itself.
    **/
    explicit XdgMimeInfo(const QFileInfo& file, bool followSymLinks=true);

    /// Returns the name of the mime type.
    QString mimeType() const;

    /// Returns the media type, eg. 'application' for mimetype 'application/pdf'
    QString mediaType() const;

    /// Returns the subtype, e.g. 'pdf' for 'application/pdf'
    QString subType() const;

    QString comment() const;

    QString localizedComment() const;

    QStringList patterns() const;

    /// Returns an icon associated with the mime type.
    QIcon   icon() const;

    /// Returns an icon associated with the mime type.
    QString iconName() const;

    QString subClassOf() const;

    bool loadFromDb(QIODevice* xml);

private:
    XdgMimeData *mData;
};


class XdgMimeInfoCache 
{
public:
    static QStringList mediatypes();
    static QStringList subtypes(const QString & media);
    static XdgMimeInfo* xdgMimeInfo(const QString & media, const QString & subtype);
    static XdgMimeInfo* xdgMimeInfo(const QString & mimetype);

private:
    static QMap<QString, QMap<QString, XdgMimeInfo*> > & cache();
};

#endif // QTXDG_XDGMIME_H
