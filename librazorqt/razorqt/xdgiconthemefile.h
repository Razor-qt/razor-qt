#ifndef XDGICONTHEMEFILE_H
#define XDGICONTHEMEFILE_H

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
/**
 * @file xdgiconthemefile.h
 * @brief Declares the class Xdgiconthemefile
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  */



/**
*@brief handles a single iconthemefile
*/
class XdgIconThemeFile
{
public:
    XdgIconThemeFile(const QString & _filename);
    ~XdgIconThemeFile();
    void parseFile();
    void searchDirs();
    QString searchIcon(const QString & _name, const QString & _context);
    QString getName();
private:
    QString themeDir;
    QString fileName;
    QString themeName;
    QString themeDesc;
    QStringList dirList;
    //this is used as follow: in the outter map the QString key is assigned to the contextMap
    //in the inner map every size is listed against the fitting subdir
    QMap<QString, QMap<int, QString> > contextMap;
    QMap<QString, QMap<int, QStringList> > fileMap;
};


#endif
