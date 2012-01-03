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

#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE 1

#include <polkitagent/polkitagent.h>
#include <polkitqt1-subject.h>

#include <QtGui/QMessageBox>

#include "policykitagent.h"
#include "policykitagentgui.h"


namespace RazorPolicykit
{

PolicykitAgent::PolicykitAgent(QObject *parent)
    : PolkitQt1::Agent::Listener(parent),
      m_inProgress(false),
      m_gui(0)
{
    PolkitQt1::UnixSessionSubject session(getpid());
    registerListener(session, "/org/razorqt/PolicyKit1/AuthenticationAgent");
}

PolicykitAgent::~PolicykitAgent()
{
    if (m_gui)
    {
        m_gui->blockSignals(true);
        m_gui->deleteLater();
    }
}


void PolicykitAgent::initiateAuthentication(const QString &actionId,
                                            const QString &message,
                                            const QString &iconName,
                                            const PolkitQt1::Details &details,
                                            const QString &cookie,
                                            const PolkitQt1::Identity::List &identities,
                                            PolkitQt1::Agent::AsyncResult *result)
{
    if (m_inProgress) 
    {
        QMessageBox::information(0, tr("PolicyKit Information"), "Another authentization in progress. Please try it again later");
        return;
    }
    m_inProgress = true;
    m_SessionIdentity.clear();

    if (m_gui)
    {
        delete m_gui;
        m_gui = 0;
    }
    m_gui = new PolicykitAgentGUI(actionId, message, iconName, details, identities); 

    foreach (PolkitQt1::Identity i, identities)
    {
        PolkitQt1::Agent::Session *session;
        session = new PolkitQt1::Agent::Session(i, cookie, result);
        Q_ASSERT(session);
        m_SessionIdentity[session] = i;
        connect(session, SIGNAL(request(QString, bool)), this, SLOT(request(QString, bool)));
        connect(session, SIGNAL(completed(bool)), this, SLOT(completed(bool)));
        connect(session, SIGNAL(showError(QString)), this, SLOT(showError(QString)));
        connect(session, SIGNAL(showInfo(QString)), this, SLOT(showInfo(QString)));
        session->initiate();
    }
}

bool PolicykitAgent::initiateAuthenticationFinish()
{
    // dunno what are those for...
    m_inProgress = false;
    return true;
}

void PolicykitAgent::cancelAuthentication()
{
    // dunno what are those for...
    m_inProgress = false;
}

void PolicykitAgent::request(const QString &request, bool echo)
{
    PolkitQt1::Agent::Session *session = qobject_cast<PolkitQt1::Agent::Session *>(sender());
    Q_ASSERT(session);
    Q_ASSERT(m_gui);

    PolkitQt1::Identity identity = m_SessionIdentity[session];
    m_gui->setPrompt(identity, request, echo);
    if (m_gui->exec())
        session->setResponse(m_gui->response());
    else
        session->setResponse("");
}

void PolicykitAgent::completed(bool gainedAuthorization)
{
    PolkitQt1::Agent::Session * session = qobject_cast<PolkitQt1::Agent::Session *>(sender());
    Q_ASSERT(session);
    
    if (!gainedAuthorization)
    {
        QMessageBox::information(0, tr("Authorization Failed"), tr("Authorization failed for some reason"));
    }

    session->result()->setCompleted();

    delete session;
    
    m_inProgress = false;
}

void PolicykitAgent::showError(const QString &text)
{
    QMessageBox::warning(0, tr("PolicyKit Error"), text);
}

void PolicykitAgent::showInfo(const QString &text)
{
    QMessageBox::information(0, tr("PolicyKit Information"), text);
}

} //namespace
