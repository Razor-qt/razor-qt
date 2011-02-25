#ifndef SUPROCESS_H
#define SUPROCESS_H

#include <QObject>

class SuProcess : public QObject
{
    Q_OBJECT

public:
    SuProcess(const QString & username, const QString & password, const QString & command);
    
    int execute();

private:
    QString m_username;
    QString m_password;
    QString m_command;
};

#endif