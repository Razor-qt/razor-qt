#include <QApplication>
#include <QtGui>
#include <razorqt/readsettings.h>
#include "razorsplash.h"
#include "razorstate.h"
#include "razormodman.h"

/**
* @file main.cpp
* @author Christopher "VdoP" Regali
* @brief just starts the sub-apps and in future maybe saves the windowstates
*/

/**
* @brief our main function doing the loading
*/

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  Readsettings initset("razor.conf");

  Razormodulemanager modman(initset.getPath() + initset.getString("module_config"));

  //i only use stack-variables here as we kill this program anyway...
  QString splashset = initset.getString("splashscreen");
  Razorsplash Splash(splashset);




  return app.exec();
}
