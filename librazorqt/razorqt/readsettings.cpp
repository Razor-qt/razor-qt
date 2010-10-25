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
QString Readsettings::getSysPath(QString _file)
{
    if (!_file.startsWith("/"))
    {
        QStringList explode = _file.split("/");
        QFile test(QDir::homePath()+"/.razor/"+explode.at(explode.count()-1));

        if (test.exists())
            return test.fileName();
        else
        {
            test.setFileName("/usr/share/razor/"+explode.at(explode.count()-1));
            if (test.exists())
            {
                qDebug() << test.fileName();
                return test.fileName();
            }
            else
            {
                test.setFileName("/usr/local/share/razor/"+explode.at(explode.count()-1));
                if (test.exists())
                {

                    qDebug() << test.fileName();
                    return test.fileName();

                }
                else
                {

                    qDebug() << "Readsettings: could not find file " << _file;
                    return "";

                }
            }
        }
    }
    else
        return _file;
}




/**
 *@brief does the actual parsing of the file
 */

void Readsettings::updateMap(QString _filename)
{
    //it does not exist? so make it users-stuff
    if (getSysPath(_filename)=="")
    {
        //first check if path exists!
        QDir dir(QDir::homePath());
	dir.mkdir(".razor");
        qDebug() << "Readsettings: created " << QDir::homePath()+"/.razor/"+_filename;
        QFile newfile(QDir::homePath()+"/.razor/"+_filename);
        QDateTime now = QDateTime::currentDateTime();
        newfile.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream addcomment(&newfile);
        addcomment << "# new file created by Readsettings on: " << now.toString() <<"\n";
        newfile.close();
    }

    QFile stl_file(getSysPath(_filename));

    configFile =getSysPath(_filename);
    stl_file.open(QIODevice::ReadOnly | QIODevice::Text);
    //create a textstream out of it and explode it into the map
    QTextStream fileStream(&stl_file);
    QString line;
    QStringList explode;
    line=fileStream.readLine();
    //do the actual parsing of the file
    do
    {
        explode=line.split("=");
        if (explode.count()>1)
            redSettings[explode[0].trimmed()]=explode[1].trimmed();
        line = fileStream.readLine();
    }
    while (!line.isNull());

    tplPath = getSysPath(_filename);
    while (!tplPath.endsWith("/") && tplPath.length() > 0)
        tplPath.chop(1);
}


/**
 * @brief constructor of this class
 */
Readsettings::Readsettings(QString _filename)
{
    qDebug() << "Readsettings: initialising... " << _filename;
    updateMap(_filename);
}

/**
 * @brief writes the settings back to the file
 */
void Readsettings::safeSettings()
{
    QMapIterator<QString, QString> Iter(redSettings);
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
            outstream << Iter.key() << "=" << Iter.value() << endl;
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
 * @brief gives back a value by key
 */
QString Readsettings::getValue(QString _key)
{
    return redSettings.value(_key);
}

/**
 * @brief sets a _key to _value.. creates of necessary
 */
void Readsettings::setValue(QString _key, QString _value)
{
    redSettings[_key] = _value;
}

#endif
