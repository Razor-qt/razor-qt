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


#include <QButtonGroup>
#include <QToolButton>
#include <QWheelEvent>
#include <QtDebug>
#include <QSignalMapper>
#include <razorqt/xfitman.h>
#include <razor-global-key-shortcuts-client/razor-global-key-shortcuts-client.h>
#include <razorqt/razorgridlayout.h>

#include <QHBoxLayout>
#include "desktopswitch.h"
#include "desktopswitchbutton.h"


Q_EXPORT_PLUGIN2(DesktopSwitch, DesktopSwitchPluginLibrary)


DesktopSwitch::DesktopSwitch(const IRazorPanelPluginStartupInfo &startupInfo) :
    QObject(),
    IRazorPanelPlugin(startupInfo),
    m_pSignalMapper(new QSignalMapper(this)),
    m_desktopCount(1)
{
    m_buttons = new QButtonGroup(this);
    connect ( m_pSignalMapper, SIGNAL(mapped(int)), this, SLOT(setDesktop(int)));

    mLayout = new RazorGridLayout(&mWidget);
    mWidget.setLayout(mLayout);
    setup();
}

void DesktopSwitch::setup()
{
    // clear current state
    foreach (QAbstractButton * b, m_buttons->buttons())
    {
        // TODO/FIXME: maybe it has to be removed from layout too?
        m_pSignalMapper->removeMappings(b);
        m_buttons->removeButton(b);
//        dynamic_cast<DesktopSwitchButton*>(b)->unregisterShortcut();
        delete b;
    }

    // create new desktop layout
    for (int i = 0; i < m_desktopCount; ++i)
    {
        QString path = QString("/panel/%1/desktop_switch/%2/desktop_%3").arg(QFileInfo(settings()->fileName()).baseName()).arg(settings()->group()).arg(i + 1);
        QString shortcut = QString("Control+F%1").arg(i + 1);

        DesktopSwitchButton * m = new DesktopSwitchButton(&mWidget, i, path, shortcut, xfitMan().getDesktopName(i, tr("Desktop %1").arg(i+1)));
        m_pSignalMapper->setMapping(m, i);
        connect(m, SIGNAL(activated()), m_pSignalMapper, SLOT(map())) ;
        mWidget.layout()->addWidget(m);
        m_buttons->addButton(m, i);
    }

    int activeDesk = qMax(xfitMan().getActiveDesktop(), 0);
    QAbstractButton * button = m_buttons->button(activeDesk);
    if (button)
        button->setChecked(true);

    connect(m_buttons, SIGNAL(buttonClicked(int)),
            this, SLOT(setDesktop(int)));

    realign();
}

DesktopSwitch::~DesktopSwitch()
{
}

void DesktopSwitch::x11EventFilter(XEvent* _event)
{
    if (_event->type == PropertyNotify)
    {
        int count = qMax(xfitMan().getNumDesktop(), 1);
        if (m_desktopCount != count)
        {
            qDebug() << "Desktop count changed from" << m_desktopCount << "to" << count;
            m_desktopCount = count;
            m_desktopNames = xfitMan().getDesktopNames();
            setup();
        }
        
        if (m_desktopNames != xfitMan().getDesktopNames())
        {
            m_desktopNames = xfitMan().getDesktopNames();
            setup();
        }

        int activeDesk = qMax(xfitMan().getActiveDesktop(), 0);
        m_buttons->button(activeDesk)->setChecked(true);
    }
}
void DesktopSwitch::setDesktop(int desktop)
{
    xfitMan().setActiveDesktop(desktop);
}


void DesktopSwitch::realign()
{
    mLayout->setEnabled(false);

    if (panel()->isHorizontal())
    {
        mLayout->setRowCount(qMin(panel()->lineCount(), mLayout->count()));
        mLayout->setColumnCount(0);
    }
    else
    {
        mLayout->setColumnCount(qMin(panel()->lineCount(), mLayout->count()));
        mLayout->setRowCount(0);
    }
    mLayout->setEnabled(true);
}

DesktopSwitchWidget::DesktopSwitchWidget():
    QFrame()
{
}

void DesktopSwitchWidget::wheelEvent(QWheelEvent *e)
{
    int max = xfitMan().getNumDesktop() - 1;
    int delta = e->delta() < 0 ? 1 : -1;
    int current = xfitMan().getActiveDesktop() + delta;

    if (current > max)
        current = 0;
    else if (current < 0)
        current = max;

    xfitMan().setActiveDesktop(current);
}
