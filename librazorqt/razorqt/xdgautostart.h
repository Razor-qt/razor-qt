#ifndef XDGAUTOSTART_H
#define XDGAUTOSTART_H


//2010 Chris "VdoP" Regali


/**
 * @file xdgautostart.h
 * @brief Declares the class xdgautostart
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  **/

#include "defs.h"
#include "xdgdesktopfile.h"

/**
 * @brief manages the xdg-correct autostart stuff
 **/

class XdgAutoStart
{
public:
    XdgAutoStart();
    ~XdgAutoStart();
    QVector<XdgDesktopFile>* get_List();
    void updateList();
    void addDirtoList(QString _dir);
    void debugAutostart();
    void cleanList();

private:
    QVector<XdgDesktopFile> autoList;
    QStringList badNames;

};


#endif
