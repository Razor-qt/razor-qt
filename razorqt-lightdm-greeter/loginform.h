#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include <QLightDM/Greeter>
#ifdef USING_LIGHTDM_QT_1
  #include <QLightDM/User>
  #include <QLightDM/Session>
#else
  #include <QLightDM/UsersModel>
  #include <QLightDM/SessionsModel>
#endif
#include <QProcess>
#include <QDialog>
#include <QKeyEvent>

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

#ifdef USING_LIGHTDM_QT_1
    void onPrompt(QString prompt, QLightDM::PromptType prompType);
#else
    void onPrompt(QString prompt, QLightDM::Greeter::PromptType promptType);
#endif
    void authenticationDone();

protected:
    virtual void keyPressEvent(QKeyEvent *event);

private:
    Ui::LoginForm *ui;

    QLightDM::Greeter m_Greeter;

    QProcess m_razorPowerProcess;

    QLightDM::UsersModel *m_UsersModel;
    QLightDM::SessionsModel *m_SessionsModel;
};

#endif // LOGINFORM_H
