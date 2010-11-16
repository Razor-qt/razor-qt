#ifndef XDGICONTHEMEFILE_CPP
#define XDGICONTHEMEFILE_CPP
/**
 * @file xdgiconthemefile.cpp
 * @brief Implements the class Xdgiconthemefile
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  */
#include "xdgiconthemefile.h"

/**
 * @brief returns the Name of the Theme
 * @return the name of the theme
 */

QString XdgIconThemeFile::getName()
{
    return themeName;
}



/**
 * @brief does the actual parsing of the file
 */
void XdgIconThemeFile::parseFile()
{
    QSettings* themefile = new QSettings(fileName, QSettings::IniFormat);
    QString lang = QLocale::system().name(); // (it_IT)
    QString country = lang.section('_', 0, 0); // (it_IT -> it)

    //get vital information first its always stored in "Icon Theme" key
    themefile->beginGroup("Icon Theme");

    //try getting the localized name, if it fails take general name
    themeName = themefile->value("Name["+country+"]").toString();
    if (themeName == "")
        themeName=themefile->value("Name").toString();
    //try getting the localized description, if fail blabla
    themeDesc = themefile->value("Comment["+country+"]").toString();
    if (themeDesc == "")
        themeDesc = themefile->value("Comment").toString();

    //qDebug() <<"tostring" << themefile->value("Directories");
    //save that dirlist if someone needs it
    dirList = themefile->value("Directories").toStringList();
    themefile->endGroup();

    for (int i=0; i < dirList.count(); i++)
    {
        themefile->beginGroup(dirList.at(i));
        //qDebug() << "Xdgicontheme: build dirlist with: " <<themefile->value("Context").toString() << " " << themefile->value("Size").toString() << " " << dirList.at(i);
        //this fills our contextMap with the fitting subdirs and contexts
        contextMap[themefile->value("Context").toString()][themefile->value("Size").toInt()]=themeDir+"/"+dirList.at(i);
        themefile->endGroup();
    }
}

/**
 * @brief constructor with params
 * @param[in] _filename the .theme file you want to parse
 */
XdgIconThemeFile::XdgIconThemeFile(QString _filename)
{
    fileName=_filename;
    themeName="";
    themeDesc="";
    QString tmp = _filename;
    while (!tmp.endsWith("/"))
        tmp.chop(1);
    themeDir=tmp;
    qDebug() << "Xdgiconthemefile: initializing... "<<themeDir<< " ";

    tmp.chop(1);
    while (!tmp.endsWith("/"))
        tmp.chop(1);
    QStringList searchpath = QIcon::themeSearchPaths();
    searchpath.append(tmp);

    QIcon::setThemeSearchPaths(searchpath);
    QIcon::setThemeName(fileName.split("/").at(fileName.split("/").count()-2));

}
/**
 * @brief search the directories for icon-files
 */

void XdgIconThemeFile::searchDirs()
{
    QSettings* themefile = new QSettings(fileName, QSettings::IniFormat);
    QStringList dirlist = themefile->allKeys();

    QStringList filters;
    filters << "*.png" << "*.svg" << "*.xpm";

    for (int i = 0; i < dirList.count(); i++)
    {
        QDir directory(themeDir+dirList.at(i));
        directory.setNameFilters(filters);
        themefile->beginGroup(dirList.at(i));

        fileMap[themefile->value("Context").toString()] [themefile->value("Size").toInt()] = directory.entryList();
        themefile->endGroup();
    }
    //qDebug() << fileMap;
}

/**
 * @brief returns a fitting icon-path for 2 params
 * @param[in] _name the that should be somehow contained in the icon
 * @param[in] _context the context that should be searched (e.g. Application, Mime-Type,...)
 * @return an absolute filepath to the most fitting icon
 */

QString XdgIconThemeFile::searchIcon(QString _name, QString _context)
{
    QString retval = "";
    for (int size = 128; size >= 16; size /= 2) // get the bigges and nicest icons we find
    {
        //first choice is .png
        if (fileMap[_context][size].contains(_name+".png"))
        {
            retval = contextMap[_context][size]+"/"+_name+".png";
            break;
        }
        //then choose .svg
        if (fileMap[_context][size].contains(_name+".svg"))
        {
            retval = contextMap[_context][size]+"/"+_name+".svg";

            break;
        }
        //at last choose .xpm this is legacy
        if (fileMap[_context][size].contains(_name+".xpm"))
        {
            retval = contextMap[_context][size]+"/"+_name+".xpm";

            break;
        }
    }
    return retval;
}



/**
 *@brief the destructor
 */
XdgIconThemeFile::~XdgIconThemeFile()
{

}




#endif
