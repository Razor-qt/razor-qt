#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE 1

#include <polkitagent/polkitagent.h>
#include <polkitqt1-agent-session.h>
#include <polkitqt1-subject.h>

#include <QtCore/QDebug>
#include <QtCore/QDebug>
#include <QtGui/QInputDialog>

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
    qWarning() << "initiateAuthentication() inprogress:" << m_inProgress;
    if (m_inProgress) 
    {
        qWarning() << "Another auth in progress"; // TODO/FIXME: message
        return;
    }
    m_inProgress = true;

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
        connect(session, SIGNAL(request(QString, bool)), this, SLOT(request(QString, bool)));
        connect(session, SIGNAL(completed(bool)), this, SLOT(completed(bool)));
        connect(session, SIGNAL(showError(QString)), this, SLOT(showError(QString)));
        connect(session, SIGNAL(showInfo(QString)), this, SLOT(showInfo(QString)));
        session->initiate();
    }
}

bool PolicykitAgent::initiateAuthenticationFinish()
{
    qWarning() << "initiateAuthenticationFinish()";
    m_inProgress = false;
    return true;
}

void PolicykitAgent::cancelAuthentication()
{
    qWarning() << "Cancelling authentication";
    m_inProgress = false;
}

void PolicykitAgent::request(const QString &request, bool echo)
{
    qWarning() << "Request: " << request;

    PolkitQt1::Agent::Session *session = qobject_cast<PolkitQt1::Agent::Session *>(sender());
    Q_ASSERT(session);
    Q_ASSERT(m_gui);

    // TODO/FIXME: map identity to session somehow - to setup correct identity in the combobox
    m_gui->setPrompt(request, echo);
    if (m_gui->exec())
        session->setResponse(m_gui->response());
    else
        session->setResponse("");
}

void PolicykitAgent::completed(bool gainedAuthorization)
{
    qWarning() << "Completed: " << gainedAuthorization;
    PolkitQt1::Agent::Session * session = qobject_cast<PolkitQt1::Agent::Session *>(sender());
    Q_ASSERT(session);

    session->result()->setCompleted();

    delete session;
    
    m_inProgress = false;
}

void PolicykitAgent::showError(const QString &text)
{
    qWarning() << "Error: " << text;
}

void PolicykitAgent::showInfo(const QString &text)
{
    qWarning() << "Info: " << text;
}

} //namespace
