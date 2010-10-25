#ifndef READSETTING_H
#define READSETTING_H

/**
 * @file readsettings.h
 * @author Christopher "VdoP" Regali
 * @brief Declares Readsettings class
 */

#include "defs.h"
/**
 * @brief Manages all the config stuff
 */
class Readsettings
{
public:
  Readsettings(QString _filename);
  //Readsettings(const Readsettings &cc);
  ~Readsettings();
  void updateMap(QString _filename);
  QString getValue(QString _key);
  void setValue(QString _key, QString _value);
  void safeSettings();
  void debugSettings() {qDebug() << redSettings;}
  QString getPath()
  {
    return tplPath;
  }
private:
  QString getSysPath(QString);
  QString configFile;
  QString tplPath;
  QMap<QString, QString> redSettings;
};

#endif
