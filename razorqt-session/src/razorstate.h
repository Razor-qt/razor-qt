#ifndef RAZORSTATE_H
#define RAZORSTATE_H

#include "defs.h"

/**
*@file razorstate.h
*@brief declares class Razorstate
*@author Christopher "VdoP" Regali
*/

/**
*@brief Manages the state of the session (logout etc) parses QProcess information
*/


class RazorState : public QObject
{

    Q_OBJECT

public:
    RazorState();
    ~RazorState();
    void addProcess(QProcess* _proc);

    void doOperation(const QString & _cmd);
public slots:
    void parseState();

private:
    QList<QProcess*> processList;
    QDBusInterface* power;
    void logout();
    void shutdown();
    void reboot();
};


#endif
