/* BEGIN_COMMON_COPYRIGHT_HEADER
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

#ifndef DESKTOPBACKGROUNDDIALOG_H
#define DESKTOPBACKGROUNDDIALOG_H

#include "ui_desktopbackgrounddialog.h"
#include "workspacemanager.h"

class RazorSettings;


class DesktopBackgroundDialog : public QDialog, public Ui::DesktopBackgroundDialog
{
    Q_OBJECT
    
public:
    DesktopBackgroundDialog(RazorSettings * cfg, int screen, QSize desktopSize, const QBrush & brush, QWidget * parent);
    ~DesktopBackgroundDialog();
    QBrush background();

private:
    QSize m_desktopSize;
    RazorWorkSpaceManager::BackgroundType m_type;
    QColor m_color;
    QString m_wallpaper;

    RazorSettings * m_config;
    int m_screen;

    void save();

private slots:
    void colorButton_clicked();
    void wallpaperButton_clicked();
    void systemButton_clicked();
    void preview();
};

#endif
