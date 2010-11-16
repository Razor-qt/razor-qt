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

/**
 * @brief this one manages the processes of the session and which modules of razor are to load
 */

class RazorModuleManager : public QObject
{
    Q_OBJECT
public:
    RazorModuleManager(QString _modconfig,QObject* _parent = 0);
    virtual ~RazorModuleManager();


public slots:
    void restartModules();


private:
    ReadSettings* modulesettings;
    RazorState* stateMan;
    QMap<QString,QProcess*> procMap;

};


#endif
