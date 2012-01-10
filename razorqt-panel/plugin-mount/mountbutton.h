/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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

#ifndef MOUNTBUTTON_H
#define MOUNTBUTTON_H

#include <QtGui/QToolButton>
#include <QtDBus/QtDBus>
#include <QtGui/QMenu>

#include <razormount/udisksinfo.h>

#include "menudiskitem.h"
#include "../panel/razorpanel.h"

class UdisksManager;

class Popup: public QWidget
{
    Q_OBJECT
public:
    explicit Popup(QWidget* parent = 0);

    MenuDiskItem *addItem(UdisksInfo *info);
    void deleteItem(UdisksInfo *info);

    int count() const { return m_items.count(); }

    void open(QPoint pos, Qt::Corner anchor=Qt::TopLeftCorner);

signals:
    void visibilityChanged(bool visible);

protected:
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private:
    void realign();

    QHash<UdisksInfo*,MenuDiskItem*> m_items;
    QPoint mPos;
    Qt::Corner mAnchor;
};



class MountButton : public QToolButton
{
    Q_OBJECT
public:
    MountButton(QWidget * parent, RazorPanel * panel);
    ~MountButton();

    enum DevAction {
            DevActionNothing,
            DevActionInfo,
            DevActionMenu
        };
    DevAction devAction() const { return mDevAction; }
    void setDevAction(DevAction devAction) { mDevAction = devAction; }

public slots:
    void showError(const QString &text);

private slots:
    void onDiskAdded(UdisksInfo *info);
    void onDiskRemoved(UdisksInfo *info);

    void showHidePopup();
    void showPopup();
    void setDown(bool down);

private:
    void initialScanDevices();
    void addMenuItem(UdisksInfo *info);

    void showMessage(const QString &text);

    Popup mPopup;

    UdisksManager *m_manager;

    RazorPanel *m_panel;
    DevAction mDevAction;
    QTimer  mPopupHideTimer;
    int mPopupHideDelay;
};

#endif
