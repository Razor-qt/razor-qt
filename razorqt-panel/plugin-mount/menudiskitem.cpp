/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
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


#include "menudiskitem.h"
#include "qtxdg/xdgicon.h"

//
//MenuDiskItem::MenuDiskItem(QWidget *parent) :
//    QWidget(parent)
//{
//    setupUi(this);
//    setMountStatus(false);
//    
//    eject->setIcon(XdgIcon::fromTheme("media-eject"));
//}

MenuDiskItem::MenuDiskItem(const DiskInfo &info, QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    m_device = info.device_name;
    m_name = info.name;
    m_devtype = info.raw_info["DEVTYPE"];
    
    eject->setIcon(XdgIcon::fromTheme("media-eject"));
    
    // TODO/FIXME: better icon handling. But how?
    if (m_devtype == "disk")
        diskIcon->setPixmap(XdgIcon::fromTheme("media-optical").pixmap(diskIcon->width()));

    setLabel(m_name);
    // TODO/FIXME: how to get mount status from DeviceInfo?
    setMountStatus(false /*is_mount*/);
}

void MenuDiskItem::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void MenuDiskItem::setLabel(const QString &text)
{
    QString label = text;
    if (label.isEmpty())
    {
        label = m_device;
    }

    label = QString("<a href=\"%1\">%1</a>").arg(label);
    diskLabel->setText(label);
}

void MenuDiskItem::setMountStatus(bool is_mount)
{
    eject->setEnabled(is_mount);
}

void MenuDiskItem::on_eject_clicked()
{
    emit ejectMedia(m_device);
}

void MenuDiskItem::on_diskLabel_linkActivated(const QString &/*link*/)
{
    emit mountMedia(m_device);
}
