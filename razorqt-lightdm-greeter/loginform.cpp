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

LoginForm::LoginForm(QWidget *parent) : 
    QWidget(parent), 
        ui(new Ui::LoginForm), 
        m_Greeter(),
        m_LoginData(&m_Greeter),
        m_razorPowerProcess(),
        m_otherUserComboIndex(-1)
{
    if (!m_Greeter.connectSync())
    {
        close();
    }

    ui->setupUi(this);

    setupAppearence(); 
    fillUserAndSessionCombos();
    setupConnections();
    initializeControls();
    ui->formFrame->adjustSize();
    adjustSize(); 
}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::setFocus(Qt::FocusReason reason)
{
    if (ui->userCombo->currentIndex() == -1)
    {
        ui->userCombo->setFocus(reason);
    }
    else if (ui->userCombo->currentIndex() == m_otherUserComboIndex)
    {
        ui->otherUserInput->setFocus(reason);
    }
    else 
    {
        ui->passwordInput->setFocus(reason);
    }
}

void LoginForm::setupAppearence()
{
    QPixmap icon(QString(SHARE_DIR) + "/graphics/rqt-2.svg");
    ui->iconLabel->setPixmap(icon.scaled(ui->iconLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->hostnameLabel->setText(m_Greeter.hostname());
}

void LoginForm::fillUserAndSessionCombos()
{
    ui->userCombo->clear(); 
    for (int i = 0; i < m_LoginData.numberOfUsers(); i++)
    {
        qDebug() << "Adding user" << m_LoginData.userFullName(i);
        ui->userCombo->addItem(m_LoginData.userFullName(i));
    }
    ui->userCombo->addItem(tr("other..."));
    m_otherUserComboIndex = ui->userCombo->count() - 1;

    ui->sessionCombo->clear();
    for (int i = 0; i < m_LoginData.numberOfSessions(); i++)
    {
        qDebug() << "Adding session" << m_LoginData.sessionFullName(i);
        ui->sessionCombo->addItem(m_LoginData.sessionFullName(i));
    }
}

void LoginForm::setupConnections()
{
    ui->userCombo->setCurrentIndex(-1);
    connect(ui->userCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(userComboCurrentIndexChanged()));
    connect(ui->otherUserInput, SIGNAL(editingFinished()), this, SLOT(otherUserEditingFinished()));
    connect(ui->loginButton, SIGNAL(clicked(bool)), this, SLOT(loginClicked()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(clearAll()));
    connect(ui->leaveButton, SIGNAL(clicked()), this, SLOT(leaveClicked()));
    connect(&m_razorPowerProcess, SIGNAL(finished(int)), this, SLOT(razorPowerFinished())); 


    connect(&m_Greeter, SIGNAL(showPrompt(QString,QLightDM::Greeter::PromptType)),
            this,       SLOT(onPrompt(QString,QLightDM::Greeter::PromptType)));

    connect(&m_Greeter, SIGNAL(authenticationComplete()), this, SLOT(authenticationComplete()));
}

void LoginForm::initializeControls()
{
    qDebug() << "showManualLoginHint:" << m_Greeter.showManualLoginHint();

    ui->sessionCombo->setCurrentIndex(m_LoginData.suggestedSession());

    if (m_Greeter.showManualLoginHint()) 
    {
        ui->userCombo->setCurrentIndex(m_otherUserComboIndex);
    }
    else 
    {
        ui->userCombo->setCurrentIndex(m_LoginData.suggestedUser());
    }

    ui->userCombo->setVisible(! m_Greeter.hideUsersHint());

    ui->otherUserInput->setVisible(ui->userCombo->currentIndex() == m_otherUserComboIndex);
    ui->otherUserInput->clear(); 
    QStringList completions;
    for (int i = 0; i < m_LoginData.numberOfUsers(); i++) 
    {
        completions << m_LoginData.userName(i);
    }
    QCompleter *completer = new QCompleter(completions, ui->otherUserInput);
    completer->setCompletionMode(QCompleter::InlineCompletion);
    ui->otherUserInput->setCompleter(completer);

    ui->passwordInput->setEnabled(false);
    ui->passwordInput->clear();
}

void LoginForm::setSessionCombo(int session_index)
{
    if (0 <= session_index && session_index < ui->sessionCombo->count())
    {
        ui->sessionCombo->setCurrentIndex(session_index);
    }
}

void LoginForm::userComboCurrentIndexChanged()
{

    qDebug() << "userComboCurrentIndexChanged:" << ui->userCombo->currentIndex();
    qDebug() << "setVisible...";
    if (ui->userCombo->currentIndex() == m_otherUserComboIndex)
    {
        ui->otherUserInput->show();
        ui->otherUserInput->setFocus();
    }
    else
    {
        ui->otherUserInput->hide();
        qDebug() << "Start authentication..";
        if (ui->userCombo->currentIndex() > -1)
        {
            setUser(m_LoginData.userName(ui->userCombo->currentIndex()));
        }
    }
    ui->formFrame->adjustSize();
    adjustSize();
}

void LoginForm::otherUserEditingFinished()
{
    if (ui->otherUserInput->text().isNull() || ui->otherUserInput->text().isEmpty())
    {
        return;
    }

    qDebug() << "Authenticating with otherUser...";

    setUser(ui->otherUserInput->text()) ;
}

void LoginForm::loginClicked()
{
    qDebug() << "loginClicked";
    m_Greeter.respond(ui->passwordInput->text().trimmed());
    ui->passwordInput->clear();
    ui->passwordInput->setEnabled(false);
}

void LoginForm::clearAll()
{
	ui->userCombo->setCurrentIndex(-1);
    initializeControls();
}

void LoginForm::leaveClicked()
{
    qDebug() << "leave";
    m_razorPowerProcess.start("razor-power");
    setEnabled(false);
}

void LoginForm::razorPowerFinished() 
{
    setEnabled(true);
}

void LoginForm::onPrompt(QString prompt, QLightDM::Greeter::PromptType promptType)
{
    qDebug() << "onPrompt";
    ui->passwordInput->setEnabled(true);
    ui->passwordInput->setFocus();
}

void LoginForm::setUser(QString user)
{
    m_user = user;
    setSessionCombo(m_LoginData.userSession(m_user));

    if (m_Greeter.inAuthentication()) 
    {
        m_Greeter.cancelAuthentication();
    }
    m_Greeter.authenticate(m_user);
}

void LoginForm::authenticationComplete()
{
    if (m_Greeter.isAuthenticated())
    {
        qDebug() << "Authenticated... starting session" << m_LoginData.sessionName(ui->sessionCombo->currentIndex());
        m_LoginData.setLastUser(m_user);
        m_Greeter.startSessionSync(m_LoginData.sessionName(ui->sessionCombo->currentIndex()));
        initializeControls(); // We should not arrive here...
    }
    else 
    {
        qDebug() << "Not authenticated";
        ui->passwordInput->clear();
        setUser(m_user);
    }
}

