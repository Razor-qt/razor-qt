#ifndef RAZORMODMAN_CPP
#define RAZORMODMAN_CPP
#include "razormodman.h"
/**
 * @file razormodman.cpp
 * @author Christopher "VdoP" Regali
 * @brief implements class Razormodulemanager
 */


/**
 * @brief the constructor, needs a valid modules.conf
 */
Razormodulemanager::Razormodulemanager(QString _modconfig, QObject* _parent)
{
  Q_UNUSED(_parent);
  stateMan = new Razorstate;
  modulesettings = new Readsettings(_modconfig);
  int modcount = modulesettings->getInt("count");
  qDebug() << "Settings geladen.. module:" << modcount;



  //maybe make this a config entry one day :)
  bool autorestart = true;

  QString num;
  QString cmd;
  for (int i = 0; i < modcount; i ++)
  {
    int power;
    num.setNum(i);
    cmd = modulesettings->getString("module_"+num);
    power = modulesettings->getInt("module_"+num+"_doespower");

    QProcess* tmp = new QProcess(this);
    tmp->start(cmd);
    if (power == 1)
      stateMan->addProcess(tmp);



    if (autorestart)
      connect(tmp, SIGNAL(finished(int)),this,SLOT(restartModules()));

    procMap[cmd]=tmp;
  }
}

/**
 * @brief this slot is called by the QProcesses if they end - they should NOT! so they get restarted here
 */
void Razormodulemanager::restartModules()
{
  for (int i = 0; i < procMap.values().count(); i++)
    if (procMap.values().at(i)->state() == QProcess::NotRunning)
      procMap.values().at(i)->start(procMap.keys().at(i));
}


/**
 * @brief the destructor, cleans up our mess
 */
Razormodulemanager::~Razormodulemanager()
{
  delete modulesettings;
  for (int i = 0; i < procMap.values().count(); i ++)
    delete procMap.values().at(i);
  delete stateMan;
}





#endif
