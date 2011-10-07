/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
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


#include <QButtonGroup>
#include <QToolButton>
#include <QWheelEvent>
#include <QtDebug>
#include <QSignalMapper>
#include <razorqt/xfitman.h>
#include <razorqxt/qxtglobalshortcut.h>


#include "desktopswitch.h"

EXPORT_RAZOR_PANEL_PLUGIN_CPP(DesktopSwitch)

DesktopSwitch::DesktopSwitch(const RazorPanelPluginStartInfo* startInfo, QWidget* parent)
    : RazorPanelPlugin(startInfo, parent),
    m_pSignalMapper(new QSignalMapper(this))
{
    setObjectName("DesktopSwitch");
    m_buttons = new QButtonGroup(this);

    QString mask("%1");
    int numDesk = qMax(xfitMan().getNumDesktop(), 1);
    int firstKey = Qt::Key_F1 ; 
    for (int i = 0; i < numDesk; ++i)
    {
        QxtGlobalShortcut * pS = new QxtGlobalShortcut(this);
        pS ->setShortcut(QKeySequence(Qt::CTRL + firstKey++));
        
        connect ( pS, SIGNAL(activated()), m_pSignalMapper, SLOT(map())) ; 
        m_pSignalMapper->setMapping( pS, i);
        
        QToolButton * m = new QToolButton(this);
        m->setText(mask.arg(i+1));
        m->setCheckable(true);
        addWidget(m);
        m_buttons->addButton(m, i);
    }
    
    connect ( m_pSignalMapper, SIGNAL(mapped(int)), this, SLOT(setDesktop(int)));


    int activeDesk = qMax(xfitMan().getActiveDesktop(), 0);
    m_buttons->button(activeDesk)->setChecked(true);

    connect(m_buttons, SIGNAL(buttonClicked(int)),
            this, SLOT(setDesktop(int)));
}

DesktopSwitch::~DesktopSwitch()
{
}

void DesktopSwitch::x11EventFilter(XEvent* _event)
{
    if (_event->type == PropertyNotify)
    {
        int activeDesk = qMax(xfitMan().getActiveDesktop(), 0);
        m_buttons->button(activeDesk)->setChecked(true);
    }
}
void DesktopSwitch::setDesktop(int desktop)
{
    xfitMan().setActiveDesktop(desktop);
}

void DesktopSwitch::wheelEvent(QWheelEvent* e)
{
    int max = xfitMan().getNumDesktop() - 1;
    int delta = e->delta() > 0 ? 1 : -1;
    int current = xfitMan().getActiveDesktop() + delta;
    
    if (current > max)
        current = 0;
    else if (current < 0)
        current = max;

    xfitMan().setActiveDesktop(current);
}


