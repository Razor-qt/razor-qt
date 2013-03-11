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

#ifndef MENUDISKITEM_H
#define MENUDISKITEM_H

#include <QFrame>

class RazorMountDevice;
class QToolButton;

class MenuDiskItem : public QFrame
{
    Q_OBJECT

public:
    explicit MenuDiskItem(RazorMountDevice *device, QWidget *parent);

    void setMountStatus(bool is_mount);

    static bool isUsableDevice(const RazorMountDevice *device);

signals:
    void error(const QString &msg);

private slots:
    void ejectButtonClicked();
    void diskButtonClicked();
    void update();
    void free();
    void mounted();
    void unmounted();

private:
    RazorMountDevice *mDevice;
    QToolButton *mDiskButton;
    QToolButton *mEjectButton;
};

#endif // MENUDISKITEM_H
