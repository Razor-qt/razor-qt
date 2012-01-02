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

    void setPrompt(const QString &text, bool echo);
    PolkitQt1::Identity identity();
    QString response();

private:
    QHash<QString,PolkitQt1::Identity> m_identityMap;
};
    
} // namespace

#endif
