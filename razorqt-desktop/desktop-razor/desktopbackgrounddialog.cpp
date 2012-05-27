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

#include <QtGui/QColorDialog>
#include <QtGui/QFileDialog>
#include <QtCore/QDirIterator>
#include <QtGui/QDesktopWidget>
#include "desktopbackgrounddialog.h"
#include <razorqt/razorsettings.h>


DesktopBackgroundDialog::DesktopBackgroundDialog(RazorSettings * cfg, int screen, int desktop, QSize desktopSize, const QBrush & brush, QWidget * parent)
    : QDialog(parent),
      m_desktopSize(desktopSize),
      m_type(RazorWorkSpaceManager::BackgroundColor),
      m_config(cfg),
      m_screen(screen),
      m_desktop(desktop)
{
    setupUi(this);
    // center it to current desktop
    move(parent->geometry().center() - geometry().center());
    
    // read current wallpaper brush
    if (brush.texture().isNull())
    {
        m_color = brush.color();
        preview();
    }
    else
    {
        QPixmap p = brush.texture().scaled(previewLabel->size(), Qt::IgnoreAspectRatio, Qt::FastTransformation);
        previewLabel->setPixmap(p);
    }

    connect(colorButton, SIGNAL(clicked()),
            this, SLOT(colorButton_clicked()));
    connect(wallpaperButton, SIGNAL(clicked()),
            this, SLOT(wallpaperButton_clicked()));
    connect(systemButton, SIGNAL(clicked()),
            this, SLOT(systemButton_clicked()));
    connect(keepAspectCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(preview()));

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    keepAspectCheckBox->setEnabled(false);
}

DesktopBackgroundDialog::~DesktopBackgroundDialog()
{
}
    
QBrush DesktopBackgroundDialog::background()
{
    QBrush background;
    switch (m_type)
    {
        case (RazorWorkSpaceManager::BackgroundPixmap):
        {
            QPixmap pm(m_wallpaper);
            pm = pm.scaled(m_desktopSize,
                           keepAspectCheckBox->isChecked() ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio,
                           Qt::SmoothTransformation);
            background.setTexture(pm);
            break;
        }
        default:
            background.setColor(m_color);
            background.setStyle(Qt::SolidPattern);
    }
    save();
    return background;
}

void DesktopBackgroundDialog::save()
{
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
        m_config->setValue("wallpaper", m_color.name());
    }
    else
    {
        m_config->setValue("wallpaper_type", "pixmap");
        m_config->setValue("wallpaper", m_wallpaper);
        m_config->setValue("keep_aspect_ratio", keepAspectCheckBox->isChecked());
    }
    m_config->endArray();  // desktops
    m_config->endArray();  // screens
    m_config->endGroup();
}

void DesktopBackgroundDialog::colorButton_clicked()
{
    QColor c = QColorDialog::getColor(m_color, this, tr("Select Background Color"));
    if (!c.isValid())
        return;
    
    keepAspectCheckBox->setEnabled(false);

    m_type = RazorWorkSpaceManager::BackgroundColor;
    m_color = c;
    preview();
}

void DesktopBackgroundDialog::wallpaperButton_clicked()
{
    QString fname = QFileDialog::getOpenFileName(this,
                                                tr("Select Wallpaper Image"),
                                                QDir::currentPath(),
                                                tr("Images (*.png *.xpm *.jpg *.jpeg *.svg)"));
    if (fname.isNull())
        return;
    
    keepAspectCheckBox->setEnabled(true);

    m_type = RazorWorkSpaceManager::BackgroundPixmap;
    m_wallpaper = fname;
    preview();
}

void DesktopBackgroundDialog::systemButton_clicked()
{
    QString fname = QFileDialog::getOpenFileName(this,
                                                tr("Select Sytem Wallpaper Image"),
                                                "/usr/share/wallpapers",
                                                tr("Images (*.png *.xpm *.jpg *.jpeg *.svg)"));
    if (fname.isNull())
        return;
    
    keepAspectCheckBox->setEnabled(true);

    m_type = RazorWorkSpaceManager::BackgroundPixmap;
    m_wallpaper = fname;
    preview();
}

void DesktopBackgroundDialog::preview()
{
    switch (m_type)
    {
        case RazorWorkSpaceManager::BackgroundPixmap:
        {
            Qt::AspectRatioMode mode = ( keepAspectCheckBox->isChecked())? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio;
            QPixmap p = QPixmap(m_wallpaper).scaled(previewLabel->size(), mode, Qt::FastTransformation);
            previewLabel->setPixmap(p);
            break;
        }
        case RazorWorkSpaceManager::BackgroundColor:
        {
            QPixmap p(1, 1);
            p.fill(m_color);
            previewLabel->setPixmap(p);
            break;
        }
    }

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
