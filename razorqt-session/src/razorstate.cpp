#ifndef RAZORSTATE_CPP
#define RAZORSTATE_CPP
#include "razorstate.h"
/**
* @file razorstate.cpp
* @brief implements Razorstate
* @author Christopher "VdoP" Regali
**/

/**
* @brief constructor, makes the DBus connection
*/
Razorstate::Razorstate()
{
  power = new QDBusInterface("org.freedesktop.Hal", "/org/freedesktop/Hal/devices/computer",
			     "org.freedesktop.Hal.Device.SystemPowerManagement",
			     QDBusConnection::systemBus());
}

/**
* @brief destructor
*/
Razorstate::~Razorstate()
{
 delete power;
}

/**
 * @brief adds a process to the list and connects it correctly with our slot
 */
void Razorstate::addProcess(QProcess* _proc)
{
  processList.append(_proc);
  connect(_proc, SIGNAL(readyRead()),this,SLOT(parseState()));
}



/**
* @brief does the actual work
*/
void Razorstate::doOperation(QString _cmd)
{
  QString cmd = _cmd;
  cmd.chop(1);
  qDebug() << "Razorstate: got output: " << cmd;
  if (cmd =="RAZOR_DO_LOGOUT")
    logout();
  else if (cmd == "RAZOR_DO_SHUTDOWN")
    shutdown();
  else if (cmd == "RAZOR_DO_REBOOT")
    reboot();
}


/**
* @brief parses the output of the Process for RAZOR_DO commands
*/
void Razorstate::parseState()
{
  qDebug() << "parsestate!";
  for (int i=0; i < processList.count();i++)
    doOperation(processList.at(i)->readLine());
}

/**
* @brief this logs us out by terminating our session
**/
void Razorstate::logout()
{
  for (int i=0; i < processList.count(); i++)
    processList.at(i)->terminate();
  QApplication::quit();
}

/**
* @brief reboot via Dbus
**/

void Razorstate::reboot()
{
 power->call("Reboot");
}

/**
* @brief shutdown via Dbus
**/

void Razorstate::shutdown()
{
  //qDebug() << "Would have shut down now";
  //debug!! no real shutdown while testing :)
  power->call("Shutdown");
}


#endif
