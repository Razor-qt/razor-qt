/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL3+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011-2012 Razor team
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

#ifndef POLICYKITAGENTGUI_H
#define POLICYKITAGENTGUI_H

#include <polkitqt1-details.h>
#include <polkitqt1-identity.h>

#include "ui_policykitagentgui.h"

namespace RazorPolicykit
{
    
class PolicykitAgentGUI : public QDialog, public Ui::PolicykitAgentGUI
{
    Q_OBJECT

public:
    PolicykitAgentGUI(const QString &actionId,
                      const QString &message,
                      const QString &iconName,
                      const PolkitQt1::Details &details,
                      const PolkitQt1::Identity::List &identities);

    void setPrompt(const PolkitQt1::Identity &identity, const QString &text, bool echo);
    PolkitQt1::Identity identity();
    QString response();

private:
    QHash<QString,PolkitQt1::Identity> m_identityMap;
};
    
} // namespace

#endif
