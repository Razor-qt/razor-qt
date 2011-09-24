/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * END_COMMON_COPYRIGHT_HEADER */
/* Based on a "MountTray" project - modified for Razor needs
    http://hatred.homelinux.net

    @date   2010-11-11
    @brief  Main application class: integrate all components

    Copyright (C) 2010 by hatred <hatred@inbox.ru>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the version 2 of GNU General Public License as
    published by the Free Software Foundation.

    For more information see LICENSE and LICENSE.ru files
*/

#ifndef MENUDISKITEM_H
#define MENUDISKITEM_H

#include "razormount/diskmonitor.h"

#include "ui_menudiskitem.h"

class MenuDiskItem : public QWidget, private Ui::MenuDiskItem
{
    Q_OBJECT

public:
//    explicit MenuDiskItem(QWidget *parent = 0);
    explicit MenuDiskItem(const DiskInfo &info, QWidget *parent = 0);

    void setLabel(const QString &text);
    void setMountStatus(bool is_mount);

    QString deviceName() const { return m_device; }

signals:
    void ejectMedia(const QString &media_dev);
    void mountMedia(const QString &media_dev);

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_eject_clicked();
    void on_diskButton_clicked();

private:
    QString m_device;
};

#endif // MENUDISKITEM_H
