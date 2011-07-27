/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
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


#ifndef QTXDG_XDGMIME_H
#define QTXDG_XDGMIME_H

#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtGui/QIcon>

class XdgMimeInfo
{
public:
    /// Constructs a RazorMimeInfo with the mimeType type.
    explicit XdgMimeInfo(const QString& mimeType);

    /// Constructs a new RazorMimeInfo that gives mime information about the given file.
    explicit XdgMimeInfo(const QFileInfo& file);

    /// Returns the name of the mime type.
    QString mimeType() const;

    /// Returns an icon associated with the mime type.
    QIcon   icon() const;

private:
    QString mType;
    QString mSubType;
};

#endif // QTXDG_XDGMIME_H
