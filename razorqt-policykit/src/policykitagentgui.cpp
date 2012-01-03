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
 
#include <qtxdg/xdgicon.h>

#include "policykitagentgui.h"

namespace RazorPolicykit
{


PolicykitAgentGUI::PolicykitAgentGUI(const QString &actionId,
                                     const QString &message,
                                     const QString &iconName,
                                     const PolkitQt1::Details &details,
                                     const PolkitQt1::Identity::List &identities)
   : QDialog(0, Qt::WindowStaysOnTopHint)
{
    setupUi(this);
    Q_UNUSED(actionId);
    Q_UNUSED(details); // it seems too confusing for end user (=me)

    messageLabel->setText(message);
    iconLabel->setPixmap(XdgIcon::fromTheme(iconName).pixmap(64, 64));
    
    foreach (PolkitQt1::Identity identity, identities)
    {
        m_identityMap[identity.toString()] = identity;
        identityComboBox->addItem(identity.toString());
    }
}

void PolicykitAgentGUI::setPrompt(const PolkitQt1::Identity &identity, const QString &text, bool echo)
{
    int ix = identityComboBox->findText(identity.toString());
    if (ix != -1)
    {
        identityComboBox->setCurrentIndex(ix);
        passwordEdit->setFocus(Qt::OtherFocusReason);
    }
    promptLabel->setText(text);
    passwordEdit->setEchoMode(echo ? QLineEdit::Normal : QLineEdit::Password);
}

PolkitQt1::Identity PolicykitAgentGUI::identity()
{
    return m_identityMap[identityComboBox->currentText()];
}

QString PolicykitAgentGUI::response()
{
    return passwordEdit->text();
}

} // namespace
