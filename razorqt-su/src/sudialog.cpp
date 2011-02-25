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
