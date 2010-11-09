#ifndef XDGDESKTOPF_CPP
#define XDGDESKTOPF_CPP
// 2010 Chris "VdoP" Regali
#include "xdgdesktopfile.h"

/**
 * @file xdgdesktopfile.cpp
 * @brief Implements the class Xdgdesktopfile
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  */


/**
 * @brief constructor of the class
 * @param[in] _filename The .desktop file to open and parse
 */
Xdgdesktopfile::Xdgdesktopfile(QString _filename)
{
    //qDebug() << "Xdgdesktopfile: initialising...";
    deskFile = new QFile(_filename);
    if (deskFile->exists())
    {
        deskFile->open(QIODevice::ReadOnly | QIODevice::Text);
        oldPath = _filename;
        while (!oldPath.endsWith("/") && oldPath.length()!=0)
            oldPath.chop(1);
        useable=true;
    }
    else
    {
        useable=false;

    }

}

/**
 * @brief stub-constructor of the class
 * only needed for use in QVector and QList
 */

Xdgdesktopfile::Xdgdesktopfile()
{

}


/**
 * @brief returns if the Xdgdesktopfile is useable
 */
bool Xdgdesktopfile::isUseable()
{
    return useable;
}



/**
 * @brief Puts the saved information to qDebug()
 */

void Xdgdesktopfile::debugDesktopFile()
{
    qDebug() << "Xdgdesktopfile: starting debug output!";
    qDebug()<<"------------------------------------------";
    QMapIterator<QString, QString> i(data);
    while (i.hasNext())
    {
        i.next();
        qDebug() << i.key() << ": " << i.value();
    }
    qDebug()<<"------------------------------------------";
    qDebug()<<"Its placed in the Categories:";
    QStringListIterator a(categories);
    while (a.hasNext())
    {
        qDebug()<< a.next();
    }
    qDebug()<<"------------------------------------------";
}

/**
 * @brief sets the filename if it was not given in the constructor
 */
void Xdgdesktopfile::setFilename(QString _filename)
{
    deskFile = new QFile(_filename);
    if (deskFile->exists())
    {
        deskFile->open(QIODevice::ReadOnly | QIODevice::Text);
        oldPath = _filename;
        while (!oldPath.endsWith("/"))
            oldPath.chop(1);
    }


}

Xdgdesktopfile::~Xdgdesktopfile()
{
    //qDebug() << "Xdgdesktopfile: dying";
}

/**
 * @brief does the actual parsing of the .desktop file
 */

void Xdgdesktopfile::parseFile()
{
    QTextStream fileStream(deskFile);
    QString deskline;
    QStringList explode;
    deskline=fileStream.readLine();
    //do the actual parsing of the file
    //is a pretty ugly xdg-1.0 implementation but itll do the job
    do
    {
        //if its categories stuff, split them up and add them to the catlist
        if (deskline.startsWith("Categories="))
        {
            deskline.remove("Categories=");
            categories = deskline.split(";");
        }
        else
            explode=deskline.split("=");
        if (explode.count()>1)
        {
            /**
             * @todo ugly hack to not care about exec clags - needs cleaning
             */
            if (explode[1].contains("%"))
                explode[1].chop(explode[1].length()  - explode[1].indexOf("%"));

            data[explode[0].trimmed()]=explode[1].trimmed();
        } //the rest is just pushed into the map!
        deskline = fileStream.readLine();
    }
    while (!deskline.isNull());
}
/**
 * @brief gets a value from the class
 * @param[in] _key the key to the info wanted, that is stored in a QMap
 * @returns the wanted info from the QMap
 */
QString Xdgdesktopfile::getValue(QString _key)
{
    return data.value(_key);
}

/**
 *@brief sets another value for any value red from the file. this overrides the file but only for this session
 *@param[in] _key the key of the map you want to change
 *@param[in] _value the new value of the key
 */

void Xdgdesktopfile::setValue(QString _key, QString _value)
{
    data[_key]=_value;
}

/**
 * @brief returns the old local path of the .desktop file
 */
QString Xdgdesktopfile::getOldPath()
{
    return oldPath;
}



#endif
