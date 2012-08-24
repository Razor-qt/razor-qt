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

// This include *must* go first
#include "desktopbackgrounddialog.h"

#include <QtGui/QPixmap>
#include <QtGui/QColor>
#include <QtGui/QDesktopWidget>

#include <razorqt/xfitman.h>
#include <razorqt/razorsettings.h>

#include "backgroundprovider.h"



BackgroundProvider::BackgroundProvider(int screen, QObject * parent)
    : QObject(parent),
      m_screen(screen),
      m_pixmap(0),
      m_ratio(Qt::IgnoreAspectRatio)
{
}

BackgroundProvider::~BackgroundProvider()
{
    if (m_pixmap)
        delete m_pixmap;
}
    
void BackgroundProvider::setFile(const QString &filename)
{
    if (m_pixmap)
        delete m_pixmap;
    m_pixmap = new QPixmap(filename);
    m_file = filename;
    save();
}

void BackgroundProvider::setPixmap(const QPixmap &pixmap)
{
    if (m_pixmap)
        delete m_pixmap;
    m_pixmap = new QPixmap(pixmap);
}

void BackgroundProvider::setScaleRatio(Qt::AspectRatioMode ratio)
{
    m_ratio =  ratio;
}

void BackgroundProvider::setColor(const QColor &color)
{
    if (m_pixmap)
        delete m_pixmap;
    m_pixmap = new QPixmap(1, 1);
    m_pixmap->fill(color);
}

QPixmap BackgroundProvider::pixmap(const QRect &rect)
{
    Q_ASSERT(m_pixmap);
    return m_pixmap->scaled(rect.size(), m_ratio, Qt::SmoothTransformation);
}

bool BackgroundProvider::gui()
{
   DesktopBackgroundDialog dia(QPixmap(*m_pixmap), m_ratio == Qt::KeepAspectRatio, 0);
   if (dia.exec())
   {
	   m_ratio = dia.keepAspectRatio() ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio;
	   m_type = dia.type();
	   if (m_type == RazorWorkSpaceManager::BackgroundPixmap)
	   {
		   m_file = dia.file();
		   setFile(m_file);
	   }
       else
       {
    	   m_color = dia.color();
    	   setColor(m_color);
           save();
       }
	   return true;
   }
   return false;
}

void BackgroundProvider::save()
{
	RazorSettings *m_config = DesktopConfig::instance()->config;
	int m_desktop = xfitMan().getActiveDesktop();

    m_config->beginGroup("razor");
    // It's strange. Event that I set array size to desktop count, there is always
    // index used. A bug in Qt? But it does not matter. I use screenCount()
    // in the array reading routine...
    m_config->beginWriteArray("screens", QApplication::desktop()->screenCount());
    m_config->setArrayIndex(m_screen);
    m_config->beginWriteArray("desktops");
    m_config->setArrayIndex(m_desktop);
    if (m_type == RazorWorkSpaceManager::BackgroundColor)
    {
        m_config->setValue("wallpaper_type", "color");
        m_config->setValue("wallpaper", m_color);
    }
    else
    {
        m_config->setValue("wallpaper_type", "pixmap");
        m_config->setValue("wallpaper", m_file);
        m_config->setValue("keep_aspect_ratio", m_ratio == Qt::KeepAspectRatio);
    }
    m_config->endArray();  // desktops
    m_config->endArray();  // screens
    m_config->endGroup();
}
