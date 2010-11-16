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
class XdgMenuLogic
{
public:
    XdgMenuLogic(int _logic, bool _not, XdgMenuLogic* _parent);
    XdgMenuLogic();
    ~XdgMenuLogic();
    QList<XdgMenuLogic*> children;
    QStringList cat_list;
    int logic;
    XdgMenuLogic* parent;
    bool hasParent;
    bool validate(XdgDesktopFile* _item);
    int makeChild(int _logic, bool _not, XdgMenuLogic* _parent);
    bool not_;
};

/**
 * @brief Is used to build up a double-linked tree which represents the menu-structure wanted
 */
class XdgMenuCross
{
public:
    XdgMenuCross(XdgMenuCross* _parent);
    XdgMenuCross();
    ~XdgMenuCross();
    QString name;
    XdgMenuCross* parent;
    bool hasParent;
    QStringList usedNames;
    bool hasInclude;
    void fillQMenu(QMenu* _target, QList< QMenu* >& _list);
    QString directory;
    XdgMenuLogic* inc_categories;
    bool fitsHere(XdgDesktopFile* _item);
    QList<XdgDesktopFile*> entries;
    QList<XdgMenuCross*> subMenu;
    void debugCross();

};









#endif
