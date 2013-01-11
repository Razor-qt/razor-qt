/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Christian Surlykke
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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

#include "loginform.h"
#include "ui_loginform.h"
#include <QtCore/QDebug>
#include <QtGui/QCompleter>
#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>
#include <QtGui/QPixmap>
#include <QtGui/QMessageBox>
#include <QtGui/QMenu>
#include <QtCore/QProcess>
#include <razorqt/razorsettings.h>

#ifdef USING_LIGHTDM_QT_1
  #include <QLightDM/System>
#endif

LoginForm::LoginForm(QWidget *parent) : QWidget(parent), ui(new Ui::LoginForm)
{
    if (!m_Greeter.connectSync())
    {
        close();
    }

    ui->setupUi(this);
    setStyleSheet(razorTheme.qss("razor-lightdm-greeter/razor-lightdm-greeter"));
    ui->hostnameLabel->setFocus();

    // Setup users
    m_UsersModel = new QLightDM::UsersModel();
    QStringList userIds;
    for (int i = 0; i < m_UsersModel->rowCount(QModelIndex()); i++)
    {
        QModelIndex index = m_UsersModel->index(i);
        QString userId =  m_UsersModel->data(index, Qt::UserRole).toString();
        userIds << userId;
    }
    QCompleter *completer = new QCompleter(userIds);
    completer->setCompletionMode(QCompleter::InlineCompletion);
    ui->userIdInput->setCompleter(completer);


    // Setup sessions
    m_SessionsModel = new QLightDM::SessionsModel();
    ui->sessionCombo->setModel(m_SessionsModel);
    for (int row = 0; row < ui->sessionCombo->model()->rowCount(); row++)
    {
        QModelIndex index = ui->sessionCombo->model()->index(row, 0);
        bool isRazorSession;
#ifdef USING_LIGHTDM_QT_1
        isRazorSession = (QString("razor") == ui->sessionCombo->model()->data(index, QLightDM::SessionsModel::IdRole).toString());
#else
        isRazorSession = (QString("razor") == ui->sessionCombo->model()->data(index, QLightDM::SessionsModel::KeyRole).toString());
#endif
        if (isRazorSession)
        {
            ui->sessionCombo->setCurrentIndex(row);
            break;
        }
    }

    QPixmap icon(QString(SHARE_DIR) + "/graphics/rqt-2.svg");
    ui->iconLabel->setPixmap(icon.scaled(ui->iconLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));


#ifdef USING_LIGHTDM_QT_1
    ui->hostnameLabel->setText(QLightDM::hostname());
    connect(&m_Greeter, SIGNAL(showPrompt(QString,QLightDM::PromptType)),
            this,       SLOT(onPrompt(QString,QLightDM::PromptType)));
#else
    ui->hostnameLabel->setText(m_Greeter.hostname());
    connect(&m_Greeter, SIGNAL(showPrompt(QString,QLightDM::Greeter::PromptType)),
            this,       SLOT(onPrompt(QString,QLightDM::Greeter::PromptType)));
#endif

    connect(ui->loginButton, SIGNAL(clicked(bool)), this, SLOT(doLogin()));

    connect(ui->cancelButton, SIGNAL(clicked()), SLOT(doCancel()));

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
    m_Greeter.authenticate(ui->userIdInput->text());
}

#ifdef USING_LIGHTDM_QT_1
    void LoginForm::onPrompt(QString prompt, QLightDM::PromptType promptType)
    {
        // We only handle password prompt
        m_Greeter.respond(ui->passwordInput->text());
    }
#else
    void LoginForm::onPrompt(QString prompt, QLightDM::Greeter::PromptType promptType)
    {
        // We only handle password prompt
        m_Greeter.respond(ui->passwordInput->text());
    }
#endif

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
