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

#include <QtGui/QToolButton>
#include <QtGui/QMessageBox>
#include <razorqt/screensaver.h>
#include <razorqxt/qxtglobalshortcut.h>

#include "panelscreensaver.h"


EXPORT_RAZOR_PANEL_PLUGIN_CPP(PanelScreenSaver)


PanelScreenSaver::PanelScreenSaver(const RazorPanelPluginStartInfo* startInfo, QWidget* parent)
    : RazorPanelPlugin(startInfo, parent)
{
    setObjectName("PanelScreenSaver");
    m_saver = new ScreenSaver(this);

    //addActions(m_saver->availableActions());
    foreach (QAction *i, m_saver->availableActions())
    {
        QToolButton * button = new QToolButton(this);
        button->setDefaultAction(i);
        addWidget(button);
    }
    
    m_key = new QxtGlobalShortcut(this);

    QKeySequence ks(Qt::CTRL + Qt::ALT + Qt::Key_L);
    if (! m_key->setShortcut(ks))
    {
        QMessageBox::information(this, tr("Global keyboard shortcut"),
                                 tr("Panel Screensaver Global shorcut: '%1' cannot be registered").arg(ks.toString()));
    }
    
    connect(m_key, SIGNAL(activated()), m_saver, SLOT(lockScreen()));
}

