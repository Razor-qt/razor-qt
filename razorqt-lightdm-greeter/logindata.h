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

#ifndef LOGIN_DATA_H
#define	LOGIN_DATA_H

#include <QtCore/QObject>
#include <QDebug>
#include <QLightDM/UsersModel>
#include <QLightDM/SessionsModel>

#include <QLightDM/Greeter>

#define LAST_USER_FILE "/var/lib/razor-lightdm-greeter/last-user"
#define LAST_USER_KEY "last-user"
#define CONFIG_FILE "/etc/lightdm/lightdm-razor-greeter.conf"

class LoginData : public QObject
{
    Q_OBJECT

public:

    LoginData(QLightDM::Greeter *greeter);
    virtual ~LoginData();

    int numberOfUsers();
    int numberOfSessions();
    
    QString userFullName(int user_index);
    QString userName(int user_index);
    QString sessionFullName(int session_index);
    QString sessionName(int session_index);
    int userSession(QString user);
    int userSession(int user_index);

    int suggestedUser();
    int suggestedSession();

    QString getLastUser();
    void setLastUser(QString user);

private:
    QString data(int index, QAbstractListModel& model, int role);
   QLightDM::Greeter *m_Greeter; 
    QLightDM::UsersModel m_UsersModel;
    QLightDM::SessionsModel m_SessionsModel;
    QString m_StatefilePath; 
};

class Msg
{
public:
    Msg();
    static void show(const QString msg);
};

#endif	/* LOGIN_DATA_H */

