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

#include <QtGui/QToolButton>
#include <QtGui/QMessageBox>
#include <QHBoxLayout>
#include <razorqt/screensaver.h>
#include <razorqt/razornotification.h>
#include <razorqxt/qxtglobalshortcut.h>

#include "panelscreensaver.h"


Q_EXPORT_PLUGIN2(screensaver, PanelScreenSaverLibrary)


PanelScreenSaver::PanelScreenSaver(const IRazorPanelPluginStartupInfo &startupInfo) :
    QObject(),
    IRazorPanelPlugin(startupInfo)
{
    mSaver = new ScreenSaver(this);

    QList<QAction*> actions = mSaver->availableActions();
    if (!actions.empty())
        mButton.setDefaultAction(actions.first());
    //mButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    
    mShortcutKey = new QxtGlobalShortcut(this);

    QKeySequence ks(Qt::CTRL + Qt::ALT + Qt::Key_L);
    if (! mShortcutKey->setShortcut(ks))
    {
        RazorNotification::notify(tr("Panel Screensaver Global shortcut: '%1' cannot be registered").arg(ks.toString()));
    }
    
    connect(mShortcutKey, SIGNAL(activated()), mSaver, SLOT(lockScreen()));
}

