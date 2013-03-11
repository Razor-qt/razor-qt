/* 
 * File:   State.h
 * Author: christian
 *
 * Created on 10. februar 2013, 08:26
 */

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

