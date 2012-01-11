/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include <QtGui/QDesktopServices>
#include "menudiskitem.h"
#include <razormount/udisksinfo.h>
#include <qtxdg/xdgicon.h>


MenuDiskItem::MenuDiskItem(UdisksInfo *info, QWidget *parent)
    : QWidget(parent),
      m_info(info)
{
    setupUi(this);
    
    eject->setIcon(XdgIcon::fromTheme("media-eject"));
    QString iconName(info->iconName());
    
    connect(info, SIGNAL(error(QString)),
            this, SIGNAL(error(QString)));

    if (!iconName.isEmpty())
        diskButton->setIcon(XdgIcon::fromTheme(iconName));
    else
        diskButton->setIcon(XdgIcon::fromTheme("drive-removable-media-usb"));

    setLabel(info->displayName() + " (" + info->fileSystem() + ")");

    setMountStatus(info->isMounted());
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
        label = m_info->displayName();;
    }

    diskButton->setText(label);
}

void MenuDiskItem::setMountStatus(bool is_mount)
{
    eject->setEnabled(is_mount);
}

void MenuDiskItem::on_eject_clicked()
{
    qWarning() << "void MenuDiskItem::on_eject_clicked()" ;
    setMountStatus(!m_info->unmount());
    qobject_cast<QWidget*>(parent())->hide();
}

void MenuDiskItem::on_diskButton_clicked()
{
    qWarning() << "void MenuDiskItem::on_diskButton_clicked()" << m_info->path();
    bool success = m_info->mount();
    setMountStatus(success);
    if (success)
        QDesktopServices::openUrl(QUrl(m_info->path()));

    qobject_cast<QWidget*>(parent())->hide();
}
