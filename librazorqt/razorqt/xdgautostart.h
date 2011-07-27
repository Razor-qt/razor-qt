#ifndef XDGAUTOSTART_H
#define XDGAUTOSTART_H


//2010 Chris "VdoP" Regali


/**
 * @file xdgautostart.h
 * @brief Declares the class xdgautostart
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  **/

#include <qtxdg/xdgdesktopfile.h>

/**
 * @brief manages the xdg-correct autostart stuff
 **/

class XdgAutoStart
{
public:
    XdgAutoStart();
    ~XdgAutoStart();
    QList<XdgDesktopFile*> list();

private:
    QList<XdgDesktopFile*> m_list;
    QStringList badNames;

    void updateList();
    void addDirtoList(const QString & _dir);
    void debugAutostart();
    void cleanList();
};


#endif
