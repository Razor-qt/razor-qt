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

private:
    Ui::LoginForm *ui;

    QLightDM::Greeter m_Greeter;

    QProcess m_razorPowerProcess;

    QLightDM::UsersModel *m_UsersModel;
    QLightDM::SessionsModel *m_SessionsModel;
};

#endif // LOGINFORM_H
