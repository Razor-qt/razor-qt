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

class Razormodulemanager : public QObject
{
  Q_OBJECT
public:
    Razormodulemanager(QString _modconfig,QObject* _parent = 0);
    virtual ~Razormodulemanager();
    
    
public slots:
    void restartModules();
    
    
private:
    Readsettings* modulesettings;
    Razorstate* stateMan;
    QMap<QString,QProcess*> procMap;
    
};


#endif
