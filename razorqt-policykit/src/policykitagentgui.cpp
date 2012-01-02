#include <qtxdg/xdgicon.h>

#include "policykitagentgui.h"

namespace RazorPolicykit
{


PolicykitAgentGUI::PolicykitAgentGUI(const QString &actionId,
                                     const QString &message,
                                     const QString &iconName,
                                     const PolkitQt1::Details &details,
                                     const PolkitQt1::Identity::List &identities)
   : QDialog()
{
    setupUi(this);
    Q_UNUSED(actionId);
//    setWindowTitle(actionId);
//    actionIdLabel->setText(actionId);
    messageLabel->setText(message);
    iconLabel->setPixmap(XdgIcon::fromTheme(iconName).pixmap(64, 64));
    
    foreach (PolkitQt1::Identity identity, identities)
    {
        m_identityMap[identity.toString()] = identity;
        identityComboBox->addItem(identity.toString());
    }
    
    foreach (QString i, details.keys())
    {
        QStringList list;
        list << i << details.lookup(i);
        detailsWidget->addTopLevelItem(new QTreeWidgetItem(list));
    }
}

void PolicykitAgentGUI::setPrompt(const QString &text, bool echo)
{
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
