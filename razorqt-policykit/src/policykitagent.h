// This is an example not a library
/***************************************************************************
 *   Copyright (C) 2008 Daniel Nicoletti <dantti85-pk@yahoo.com.br>        *
 *   Copyright (C) 2009 Radek Novacek    <rnovacek@redhat.com>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#ifndef POLICYKITAGENT_H
#define POLICYKITAGENT_H

#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE 1

#include <polkitqt1-agent-session.h>
#include <polkitqt1-identity.h>
#include <polkitqt1-details.h>
#include <polkitqt1-agent-listener.h>

#include <QtDBus/QDBusContext>
#include <QtGui/QApplication>



namespace RazorPolicykit
{

class PolicykitAgentGUI;

class PolicykitAgent : public PolkitQt1::Agent::Listener
{
    Q_OBJECT

public:
    PolicykitAgent(QObject *parent = 0);
    ~PolicykitAgent();

public slots:
    void initiateAuthentication(const QString &actionId,
                                const QString &message,
                                const QString &iconName,
                                const PolkitQt1::Details &details,
                                const QString &cookie,
                                const PolkitQt1::Identity::List &identities,
                                PolkitQt1::Agent::AsyncResult *result);
    bool initiateAuthenticationFinish();
    void cancelAuthentication();

    void request(const QString &request, bool echo);
    void completed(bool gainedAuthorization);
    void showError(const QString &text);
    void showInfo(const QString &text);

private:
    bool m_inProgress;
    PolicykitAgentGUI * m_gui;
};

} // namespace

#endif
