#include "loginform.h"
#include "ui_loginform.h"
#include <QDebug>
#include <QLightDM/Session>
#include <QLightDM/User>
#include <QLightDM/System>
#include <QCompleter>
#include <QAbstractListModel>
#include <QModelIndex>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QPixmap>
#include <QMessageBox>
#include <QModelIndex>
#include <QMenu>
#include <QProcess>
#include <razorqt/razorsettings.h>

LoginForm::LoginForm(QWidget *parent) : QWidget(parent), ui(new Ui::LoginForm)
{
    if (! m_Greeter.connectSync())
    {
        close();
    }

    ui->setupUi(this);
    setStyleSheet(razorTheme->qss("razor-lightdm-greeter/razor-lightdm-greeter"));
    ui->hostnameLabel->setFocus();

    // Setup users
    QLightDM::UsersModel *model = QLightDM::users();
    QStringList userIds;
    for (int i = 0; i < model->rowCount(QModelIndex()); i++)
    {
        QModelIndex index = model->index(i);
        QString userId =  model->data(index, Qt::UserRole).toString();
        userIds << userId;
    }
    QCompleter *completer = new QCompleter(userIds);
    completer->setCompletionMode(QCompleter::InlineCompletion);
    ui->userIdInput->setCompleter(completer);


    // Setup sessions
    ui->sessionCombo->setModel(QLightDM::sessions());
    for (int row = 0; row < ui->sessionCombo->model()->rowCount(); row++)
    {
        QModelIndex index = ui->sessionCombo->model()->index(row, 0);
        if (QString("Razor Desktop") ==  ui->sessionCombo->model()->data(index, Qt::DisplayRole).toString())
        {
            ui->sessionCombo->setCurrentIndex(row);
            break;
        }
    }

    QPixmap icon(QString(SHARE_DIR) + "/graphics/rqt-2.svg");
    ui->iconLabel->setPixmap(icon.scaled(ui->iconLabel->size()));

    ui->hostnameLabel->setText(QLightDM::hostname());

    connect(ui->loginButton, SIGNAL(pressed()), this, SLOT(doLogin()));
    connect(ui->loginButton, SIGNAL(clicked(bool)), this, SLOT(doLogin()));
    qDebug() << "Default: " << ui->loginButton->isDefault();

    connect(ui->cancelButton, SIGNAL(clicked()), SLOT(doCancel()));
    connect(&m_Greeter, SIGNAL(showPrompt(QString,QLightDM::PromptType)), this, SLOT(onPrompt(QString,QLightDM::PromptType)));
    connect(&m_Greeter, SIGNAL(authenticationComplete()), this, SLOT(authenticationDone()));


    connect(ui->leaveButton, SIGNAL(clicked()), SLOT(doLeave()));
    connect(&m_razorPowerProcess, SIGNAL(finished(int)), this, SLOT(razorPowerDone()));
}




LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::doLogin()
{
    qDebug() << "In login";
    m_Greeter.authenticate(ui->userIdInput->text());
}

void LoginForm::onPrompt(QString prompt, QLightDM::PromptType promptType)
{
    // We only handle password prompt
    m_Greeter.respond(ui->passwordInput->text());
}

void LoginForm::authenticationDone()
{
    if (m_Greeter.isAuthenticated())
    {
        QString session = ui->sessionCombo->itemData(ui->sessionCombo->currentIndex(), QLightDM::SessionsModel::IdRole).toString();
        m_Greeter.startSessionSync(session);
    }
    else
    {
        doCancel();
    }
}


void LoginForm::doCancel()
{
    ui->hostnameLabel->setFocus();
    ui->userIdInput->setText("");
    ui->passwordInput->setText("");
}

void LoginForm::doLeave()
{
    m_razorPowerProcess.start("razor-power");
    setEnabled(false);
}

void LoginForm::razorPowerDone() {
    setEnabled(true);
}

void LoginForm::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        qDebug() << "Enter hit...";
        emit ui->loginButton->click();
    }
}
