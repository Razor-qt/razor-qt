#ifndef RAZORMODMAN_H
#define RAZORMODMAN_H

#include "defs.h"
#include <razorqt/readsettings.h>
#include "razorstate.h"
/**
 * @file razormodman.h
 * @author Christopher "VdoP" Regali
 * @brief declares class Razormodulemanager
 */

struct Module
{
    bool power;
    QProcess * process;
};
typedef QMap<QString,Module> ModulesMap;
typedef QMapIterator<QString,Module> ModulesMapIterator;

/**
 * @brief this one manages the processes of the session and which modules of razor are to load
 */

class RazorModuleManager : public QObject
{
    Q_OBJECT
public:
    RazorModuleManager(QObject * parent = 0);
    virtual ~RazorModuleManager();


public slots:
    void restartModules(int exitCode, QProcess::ExitStatus exitStatus);

private:
    bool autorestart;
    ReadSettings* cfg;
    RazorState* stateMan;

    ModulesMap procMap;
	QList<QProcess*> autostartList;

private slots:
	void autoStartSingleShot();
};


#endif
