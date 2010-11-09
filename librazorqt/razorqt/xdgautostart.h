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
#include "xdgenv.h"
#include "xdgdesktopfile.h"

/**
 * @brief manages the xdg-correct autostart stuff
 **/

class Xdgautostart
{
public:
    Xdgautostart(Xdgenv* _env);
    Xdgautostart();
    ~Xdgautostart();
    QVector<Xdgdesktopfile>* get_List();
    void updateList();
    void addDirtoList(QString _dir);
    void debugAutostart();
    void cleanList();

private:
    QVector<Xdgdesktopfile> autoList;
    Xdgenv* autoEnv;
    QStringList badNames;

};


#endif
