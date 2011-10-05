/* BEGIN_COMMON_COPYRIGHT_HEADER
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

#include "sudialog.h"

#include <QByteArray>



SuDialog::SuDialog(const QString & user, const QString & authUser, const QString & command)
    : QDialog()
{
	setupUi(this);

    setWindowTitle(tr("Run as %1").arg(user));
    
    usernameEdit->setText(user);
    passwordEdit->setFocus(Qt::OtherFocusReason);

    QString prompt;
    if (authUser == "root") {
        prompt = tr("The action you requested needs <b>root privileges</b>. "
                    "Please enter <b>root's</b> password below.");
    } else {
        prompt = tr("The action you requested needs additional privileges. "
                    "Please enter the password for <b>%1</b> below.").arg(authUser);
    }
    infoLabel->setText(prompt);
    commandLabel->setText(command);
}

QString SuDialog::password()
{
    return passwordEdit->text();
}
