#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include <QLightDM/Greeter>
#include <QProcess>

namespace Ui {
class LoginForm;
}

class LoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = 0);
    ~LoginForm();

public slots:
    void doLogin();
    void doCancel();
    void doLeave();
    void razorPowerDone();
    void onPrompt(QString prompt, QLightDM::PromptType promptType);
    void authenticationDone();

private:
    Ui::LoginForm *ui;

    QLightDM::Greeter m_Greeter;

    QProcess m_razorPowerProcess;
};

#endif // LOGINFORM_H
