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


QuickLaunchLayout::QuickLaunchLayout(QWidget * parent, RazorPanel *panel)
    : QGridLayout(parent),
      m_panel(panel)
{
    setSpacing(0);
    setContentsMargins(0, 0, 0, 0);

    connect(m_panel, SIGNAL(layoutDirectionChanged(QBoxLayout::Direction direction)),
            this, SLOT(relayout()));
    connect(m_panel, SIGNAL(panelRealigned()),
            this, SLOT(relayout()));
}

void QuickLaunchLayout::addWidget(QuickLaunchButton *b)
{
    m_buttons << b;
    relayout();
}

void QuickLaunchLayout::removeWidget(QuickLaunchButton *b)
{
    m_buttons.removeAll(b);
    relayout();
}

void QuickLaunchLayout::swapButtons(QuickLaunchButton * b1, QuickLaunchButton *b2)
{
    int ix1, ix2;
    int tmp1 = indexOf(b1);
    int tmp2 = indexOf(b2);

    ix1 = (tmp1<tmp2) ? tmp1 : tmp2;
    ix2 = (tmp1<tmp2) ? tmp2 : tmp1;
   
    m_buttons.swap(ix1, ix2);
    relayout();
}

Qt::Orientations QuickLaunchLayout::expandingDirections() const
{
    return m_panel->isHorizontal() ? Qt::Horizontal : Qt::Vertical;
}

// I know it's ugly and too expensive (to use this brute-force method)
// but now it all works (horizontal and vertical mode), moving of buttons,
// removing etc. And mainly - it places buttons into matrix if there is
// free space for it.
void QuickLaunchLayout::relayout()
{
    int size = m_panel->isHorizontal() ? qobject_cast<QWidget*>(parent())->height() : qobject_cast<QWidget*>(parent())->width();
    int div = size / QUICKLAUNCH_BUTTON_SIZE;
    int col = 0;
    int row = 0;
    
    QLayoutItem *child;
    while ((child = takeAt(0)) != 0)
    {
        delete child;
    }
    
    foreach (QuickLaunchButton *b, m_buttons)
    {
        QGridLayout::addWidget(b, row, col);
        if (m_panel->isHorizontal())
        {
            ++row;
            if (row == div)
            {
                row = 0;
                ++col;
            }
        }
        else
        {
            ++col;
            if (col == div)
            {
                col = 0;
                ++row;
            }
        }
    }
}
