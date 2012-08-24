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
#include "desktopbackgrounddialog.h"


DesktopBackgroundDialog::DesktopBackgroundDialog(const QPixmap &preview, bool keepAspectRatio, QWidget * parent)
    : QDialog(parent),
      m_type(RazorWorkSpaceManager::BackgroundColor)
{
    setupUi(this);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    QPixmap p = preview.scaled(previewLabel->size(), Qt::IgnoreAspectRatio, Qt::FastTransformation);
    previewLabel->setPixmap(p);

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
    keepAspectCheckBox->setChecked(keepAspectRatio);
}

DesktopBackgroundDialog::~DesktopBackgroundDialog()
{
}
    
bool DesktopBackgroundDialog::keepAspectRatio()
{
	if (!keepAspectCheckBox->isEnabled())
		return true;
	return keepAspectCheckBox->isChecked();
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
            p = p.scaled(previewLabel->size());
            previewLabel->setPixmap(p);
            break;
        }
    }

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
