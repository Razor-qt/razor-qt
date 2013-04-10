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
#include <razor-global-key-shortcuts-client/razor-global-key-shortcuts-client.h>

#include "panelscreensaver.h"


Q_EXPORT_PLUGIN2(screensaver, PanelScreenSaverLibrary)

#define DEFAULT_SHORTCUT "Control+Alt+L"


PanelScreenSaver::PanelScreenSaver(const IRazorPanelPluginStartupInfo &startupInfo) :
    QObject(),
    IRazorPanelPlugin(startupInfo)
{
    mSaver = new ScreenSaver(this);

    QList<QAction*> actions = mSaver->availableActions();
    if (!actions.empty())
        mButton.setDefaultAction(actions.first());
    //mButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    mShortcutKey = GlobalKeyShortcut::Client::instance()->addAction(QString(), QString("/panel/%1/screen_saver/%2/lock").arg(QFileInfo(settings()->fileName()).baseName()).arg(settings()->group()), tr("Lock Screen"), this);
    if (mShortcutKey)
    {
        connect(mShortcutKey, SIGNAL(activated()), mSaver, SLOT(lockScreen()));

        if (mShortcutKey->shortcut().isEmpty())
        {
            mShortcutKey->changeShortcut(DEFAULT_SHORTCUT);
            if (mShortcutKey->shortcut().isEmpty())
            {
                RazorNotification::notify(tr("Panel Screensaver: Global shortcut '%1' cannot be registered").arg(DEFAULT_SHORTCUT));
            }
        }
    }
}

#undef DEFAULT_SHORTCUT
