#ifndef XDGDESKTOPF_H
#define XDGDESKTOPF_H

#include "defs.h"
/**
 * @file xdgdesktopfile.h
 * @brief Declares the class Xdgdesktopfile
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  */


/**
 * @brief Is used to parse .desktop file and store the parsed information
 */
class Xdgdesktopfile
{

public:
    Xdgdesktopfile(QString _filename);
    Xdgdesktopfile();
    void setFilename(QString _filename);
    ~Xdgdesktopfile();
    void parseFile();
    QString getValue(QString _key);
    bool isUseable();
    void debugDesktopFile();
    QString getOldPath();
    QStringList categories;
    bool useable;
    void setValue(QString _key, QString _value);
private:
    QString oldPath;
    QFile* deskFile;
    QMap<QString, QString> data;
};



#endif
