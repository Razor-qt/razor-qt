#ifndef READSETTINGS_CPP
#define READSETTINGS_CPP
#include "readsettings.h"


/**
 * @file readsettings.cpp
 * @author Christopher "VdoP" Regali
 * @brief implements the Readsettings class
 */


/**
 * @brief destructor
 */
ReadSettings::~ReadSettings()
{

}

/**
 * @brief this looks in all the usual system paths for our file
 */
QString ReadSettings::getSysPath(const QString & fileName)
{
    if (fileName.startsWith("/"))
    {
        return fileName;
    }

    QStringList explode = fileName.split("/");
    QString lastPart = explode.at(explode.count()-1);
    QString pathTemplate("%1/%2");
    QString test;

    // a hack to simplify the multiple file location tests
#define TESTFILE(prefix)                            \
    test = pathTemplate.arg( (prefix), lastPart );  \
    if (QFile::exists(test))                        \
    {                                               \
        qDebug() << "Readsettings: Config found in" << (prefix) << lastPart; \
        return test;                                \
    }

    // home dir
    TESTFILE(QDir::homePath() + "/.razor/");

#ifdef SHARE_DIR
    // test for non-standard install dirs - useful for development for example
    TESTFILE(SHARE_DIR)
#else
#warning SHARE_DIR is not defined. Config will not be searched in the CMAKE_INSTALL_PREFIX
    // this is ugly and it should be used only in a very special cases
    // standard cmake build contains the SHARE_DIR defined in the top level CMakeLists.txt
    //
    // /usr/local/ goes first as it's usually before the /usr in the PATH etc.
    TESTFILE("/usr/local/share/razor/");
    TESTFILE("/usr/share/razor/")
#endif

    // die silently
    //! \todo TODO/FIXME: shouldn't be here something like "wraning and close"?
    qDebug() << "Readsettings: could not find file " << fileName;
    return "";
}




/**
 *@brief does the actual parsing of the file
 */

void ReadSettings::updateMap(const QString & fileName)
{
    configFile = getSysPath(fileName);

    //it does not exist? so make it users-stuff
    if (configFile.isEmpty())
    {
        //first check if path exists!
        QDir dir(QDir::homePath());
        dir.mkdir(".razor");
        QString localFName(QDir::homePath()+"/.razor/"+fileName);
        qDebug() << "Readsettings: created " << localFName;
        QFile newfile(localFName);
        QDateTime now = QDateTime::currentDateTime();
        newfile.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream addcomment(&newfile);
        addcomment << "# new file created by Readsettings on: " << now.toString() <<"\n";
        //! \todo TODO/FIXME: some default config for ~ ?
        newfile.close();
    }

    if (! QFile::exists(configFile))
    {
        errMsg = "File does not exists: " + configFile;
        return;
    }

    QSettings s(configFile, QSettings::IniFormat);
    foreach (QString key, s.allKeys())
    {
        settings[key] = s.value(key);
    }

    QFileInfo fi(configFile);
    // all other code expects that getPath will contain the trailing /
    tplPath = fi.absolutePath() + "/";
}


/**
 * @brief constructor of this class
 */
ReadSettings::ReadSettings(const QString & fileName)
{
    qDebug() << "Readsettings: initialising... " << fileName;
    updateMap(fileName);
}

/**
 * @brief writes the settings back to the file
 */
void ReadSettings::saveSettings()
{
    QSettings s(configFile, QSettings::IniFormat);
    SettingsMapIterator Iter(settings);

    while (Iter.hasNext())
    {
        Iter.next();
        s.setValue(Iter.key(), Iter.value());
    }
    qDebug() << "Readsettings: written to " << configFile;
}



/**
 * @brief gives back a value as a string by key
 */
QString ReadSettings::getString(const QString & key)
{
    return settings.value(key).toString();
}

/**
 * @brief gives back a value as an int by key
 */
int ReadSettings::getInt(const QString & key)
{
    bool ok;
    int ret = settings.value(key).toInt(&ok);
    if (!ok)
    {
        qDebug() << "Readsettings::getInt ERROR: cannot convert" << key << settings.value(key) << " to int";
        //! \todo TODO/FIXME: some nice error handling would be fine...
        return -1;
    }
    return ret;
}

/**
 * @brief sets a _key to _value.. creates of necessary
 */
void ReadSettings::setValue(const QString & key, const QVariant & value)
{
    settings[key] = value;
}

#endif
