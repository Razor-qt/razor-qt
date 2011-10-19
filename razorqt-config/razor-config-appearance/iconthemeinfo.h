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

#ifndef ICONTHEMEINFO_H
#define ICONTHEMEINFO_H

#include <QtCore/QObject>
#include <QtGui/QIcon>
#include <QtCore/QDir>
#include <QtCore/QSettings>

class IconThemeInfo
{
public:
    IconThemeInfo(const QDir &dir);

    QString fileName() const { return mFileName; }
    QString name() const { return mName; }
    QString text() const { return mText; }
    QString comment() const { return mComment; }

    bool isValid() const { return mValid; }
    bool isHidden() const { return mHidden; }
    QIcon icon(const QString &iconName) const;
private:
    QString mFileName;
    QString mName;
    QString mText;
    QString mComment;
    QString mActionsDir;

    bool mValid;
    bool mHidden;

    void load(const QString &fileName);
    void loadDirsInfo(QSettings &file, const QString &path);
};


#endif // ICONTHEMEINFO_H
