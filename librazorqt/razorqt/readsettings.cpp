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
Readsettings::~Readsettings()
{

}

/**
 * @brief this looks in all the usual system paths for our file
 */
QString Readsettings::getSysPath(const QString & fileName)
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

void Readsettings::updateMap(const QString & fileName)
{
    configFile = getSysPath(fileName);

    //it does not exist? so make it users-stuff
    if (configFile.isEmpty())
    {
        //first check if path exists!
        QDir dir(QDir::homePath());
        dir.mkdir(".razor");
        qDebug() << "Readsettings: created " << QDir::homePath()+"/.razor/"+fileName;
        QFile newfile(QDir::homePath()+"/.razor/"+fileName);
        QDateTime now = QDateTime::currentDateTime();
        newfile.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream addcomment(&newfile);
        addcomment << "# new file created by Readsettings on: " << now.toString() <<"\n";
        //! \todo TODO/FIXME: some default config for ~ ?
        newfile.close();
    }

    QSettings s(configFile, QSettings::IniFormat);
    foreach (QString key, s.allKeys())
    {
        settings[key] = s.value(key);
    }
#if 0
    QFile stl_file(configFile);
    stl_file.open(QIODevice::ReadOnly | QIODevice::Text);
    //create a textstream out of it and explode it into the map
    QTextStream fileStream(&stl_file);
    QString line;
    QStringList explode;
    line = fileStream.readLine();
    //do the actual parsing of the file
    do
    {
        explode=line.split("=");
        if (explode.count() > 1)
            redSettings[explode[0].trimmed()] = explode[1].trimmed();
        line = fileStream.readLine();
    }
    while (!line.isNull());
#endif

    QFileInfo fi(configFile);
    // all other code expects that getPath will contain the trailing /
    tplPath = fi.absolutePath() + "/";
}


/**
 * @brief constructor of this class
 */
Readsettings::Readsettings(const QString & fileName)
{
    qDebug() << "Readsettings: initialising... " << fileName;
    updateMap(fileName);
}

/**
 * @brief writes the settings back to the file
 */
void Readsettings::saveSettings()
{
    SettingsMapIterator Iter(settings);
    QFile savetoFile(configFile);
    if (savetoFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QString buffer;
        QTextStream outstream(&savetoFile);
        //clears the file
        buffer = outstream.readAll();

        while (Iter.hasNext())
        {
            Iter.next();
            outstream << Iter.key() << "=" << Iter.value().toString() << endl;
        }
        savetoFile.close();
        qDebug() << "Readsettings: written to " << configFile;
    }
    else
    {
        qDebug() << "Readsettings: couldnt write to " << configFile;
    }
}



/**
 * @brief gives back a value as a string by key
 */
QString Readsettings::getString(const QString & key)
{
    return settings.value(key).toString();
}

/**
 * @brief gives back a value as an int by key
 */
int Readsettings::getInt(const QString & key)
{
    bool ok;
    int ret = settings.value(key).toInt(&ok);
    if (!ok)
    {
        qDebug() << "Readsettings::getInt ERROR: cannot convert" << key << settings.value(key) << " to int";
        //! \todo TODO/FIXME: some nice error handlinf would be fine...
        return -1;
    }
    return ret;
}

/**
 * @brief sets a _key to _value.. creates of necessary
 */
void Readsettings::setValue(const QString & key, const QVariant & value)
{
    settings[key] = value;
}

#endif
