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
class ReadSettings
{

public:
    ReadSettings(const QString & fileName);
    //Readsettings(const Readsettings &cc);
    ~ReadSettings();

    //! Get a QString from the setting map
    QString getString(const QString & key);
    /*! Get an int from the setting map.
    It returns -1 and prints a debug message if there is an converion error
    */
    int getInt(const QString & key);

    void setValue(const QString & key, const QVariant & value);
    void saveSettings();

    void debugSettings()
    {
        qDebug() << settings;
    }

    QString getPath()
    {
        return tplPath;
    }
private:

    //! Universal setting map structure
    typedef QHash<QString, QVariant> SettingsMap;
    //! Settings map iterator
    typedef QHashIterator<QString, QVariant> SettingsMapIterator;

    QString configFile;
    QString tplPath;
    SettingsMap settings;

    void updateMap(const QString & fileName);
    QString getSysPath(const QString & fileName);
};

#endif
