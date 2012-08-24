/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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

#ifndef BACKGROUNDPROVIDER_H
#define BACKGROUNDPROVIDER_H


#include <QtCore/QObject>
#include "workspacemanager.h"

class QPixmap;
class QColor;
class QRect;


class BackgroundProvider : public QObject
{
    Q_OBJECT
    
public:
    BackgroundProvider(int screen, QObject * parent);
    ~BackgroundProvider();

    void setFile(const QString &filename);
    void setPixmap(const QPixmap &pixmap);
    void setScaleRatio(Qt::AspectRatioMode ratio);
    void setColor(const QColor &brush);

    QPixmap pixmap(const QRect &rect);

    bool gui();

private:

    int m_screen;
    QPixmap *m_pixmap;
    QString m_file;
    QString m_color;
    RazorWorkSpaceManager::BackgroundType m_type;
    Qt::AspectRatioMode m_ratio;

    void save();
};

#endif
