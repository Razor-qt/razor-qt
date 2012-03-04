#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include <QLightDM/Greeter>
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
    void onPrompt(QString prompt, QLightDM::PromptType promptType);
    void authenticationDone();

protected:
    virtual void keyPressEvent(QKeyEvent *event);

private:
    Ui::LoginForm *ui;

    QLightDM::Greeter m_Greeter;

    QProcess m_razorPowerProcess;
};

#endif // LOGINFORM_H
