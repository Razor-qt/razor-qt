#ifndef XDGMENUCROSS_H
#define XDGMENUCROSS_H

// 2010 Chris "VdoP" Regali
// Implements some Logic and the menu-build from the XDG-Menufile
// is controlled via Xdgmenu-class

/**
 * @file xdgmenucross.h
 * @brief Declares the classes "Xdgmenulogic" and "Xdgmenucross"
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  */


#define XDGMENULOGIC_AND 1
#define XDGMENULOGIC_OR 2

#include "xdgiconthememanager.h"
#include "xdgdesktopfile.h"
#include "defs.h"

/**
 * @brief Does all the logic needed for the XDG-Standart in the menus
 */
class Xdgmenulogic
{
public:
    Xdgmenulogic(int _logic, bool _not, Xdgmenulogic* _parent);
    Xdgmenulogic();
    ~Xdgmenulogic();
    QList<Xdgmenulogic*> children;
    QStringList cat_list;
    int logic;
    Xdgmenulogic* parent;
    bool hasParent;
    bool validate(Xdgdesktopfile* _item);
    int makeChild(int _logic, bool _not, Xdgmenulogic* _parent);
    bool not_;
};

/**
 * @brief Is used to build up a double-linked tree which represents the menu-structure wanted
 */
class Xdgmenucross
{
public:
    Xdgmenucross(Xdgmenucross* _parent);
    Xdgmenucross();
    ~Xdgmenucross();
    QString name;
    Xdgmenucross* parent;
    bool hasParent;
    QStringList usedNames;
    bool hasInclude;
    void fillQMenu(QMenu* _target, QList< QMenu* >& _list);
    QString directory;
    Xdgmenulogic* inc_categories;
    bool fitsHere(Xdgdesktopfile* _item);
    QList<Xdgdesktopfile*> entries;
    QList<Xdgmenucross*> subMenu;
    void debugCross();

};









#endif
