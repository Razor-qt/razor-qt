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

#include "quicklaunchlayout.h"
#include "quicklaunchbutton.h"


QuickLaunchLayout::QuickLaunchLayout(QWidget * parent)
    : QGridLayout(parent)
{
    setSpacing(0);
    setContentsMargins(0, 0, 0, 0);
}

void QuickLaunchLayout::swapButtons(QuickLaunchButton * b1, QuickLaunchButton *b2)
{
    int ix1, ix2;
    int tmp1 = indexOf(b1);
    int tmp2 = indexOf(b2);

    ix1 = (tmp1<tmp2) ? tmp1 : tmp2;
    ix2 = (tmp1<tmp2) ? tmp2 : tmp1;

    // takeAt order is important here. Bigger index *must* go first
    QWidgetItem *item2 = reinterpret_cast<QWidgetItem*>(takeAt(ix2));
    QWidgetItem *item1 = reinterpret_cast<QWidgetItem*>(takeAt(ix1));
//    insertItem(ix1, item2);
//    insertItem(ix2, item1);
}

QList<QuickLaunchButton*> QuickLaunchLayout::buttons()
{
    QList<QuickLaunchButton*> ret;
    for (int i = 0; i < count(); ++i)
    {
        ret.append(reinterpret_cast<QuickLaunchButton*>(itemAt(i)->widget()));
    }
    return ret;
}

