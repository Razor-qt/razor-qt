#ifndef XDGMENUCROSS_CPP
#define XDGMENUCROSS_CPP

#define QNODEBUGCROSS

#include "xdgmenucross.h"

/**
 * @file xdgmenucross.cpp
 * @brief Implements the class "Xdgmanager.h"
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  */



/**
 * @brief The constructor of the class
 * initializes the new menu-crosspoint
 * @param[in] _parent The parent menucross if there is any
 */
XdgMenuCross::XdgMenuCross(XdgMenuCross* _parent)
{
    qDebug() << "Xdgmenucross: initialising with parent...";
    parent=_parent;
    hasInclude=false;
}


/**
 * @brief The constructor of the class,
 * Initializes a new parent-less menu-crosspoint (best for creating a root-point)
 */

XdgMenuCross::XdgMenuCross()
{
    qDebug() << "Xdgmenucross: initialising without parent...";
    parent=NULL;
    hasParent=false;
    hasInclude=false;
}

/**
 * @brief The destructor
 * @todo needs to be done!
 */
XdgMenuCross::~XdgMenuCross()
{
}

/**
 * @brief Constructs a QMenu according to our Xdgmenucross-tree
 * @param[in] _target the targetpointer where the tree shall be made
 * @param[in] _list a pointerlist that stores all the pointers to the newly created menus.. useful since qt does not provide it directly elsewhere
 */

void XdgMenuCross::fillQMenu(QMenu* _target, QList< QMenu* >& _list)
{
    QString lang = QLocale::system().name(); // (it_IT)
    QString country = lang.section('_', 0, 0); // (it_IT -> it)

    //add a new menu for us and save the pointer
    //the title is now set to directory this is not the actual title, were gonna fill it with life later.. this is not the real place to do it
    /**
     * @todo This is pretty ugly here! Xdgmenu should read and hold those .directory entries and fill this structure with life so we just give the directory-filename as title now and push the work towards the Xdgmenu... but its nasty anyway. on the otherside recursion seems the perfect way to do this task.. i dont know
     */
    QMenu* newCurrent = _target->addMenu(directory);
    //ok now fill in some "action" ;)
    _list.append(newCurrent);
    for (int i = 0; i < entries.count(); i++)
    {
        XdgDesktopFile* tmp = entries.at(i);
        QAction* newact;

        if (tmp->getValue("Name["+country+"]") != "")
            newact = new QAction((QIcon)tmp->getValue("Icon"),tmp->getValue("Name["+country+"]"),newCurrent);
        else
            newact = new QAction((QIcon)tmp->getValue("Icon"),tmp->getValue("Name"),newCurrent);

        if (tmp->getValue("Comment["+country+"]") != "")
            newact->setToolTip(tmp->getValue("Comment["+country+"]"));
        else
            newact->setToolTip(tmp->getValue("Comment"));


        newact->setData(tmp->getValue("Exec"));
        newCurrent->addAction(newact);
    }

    //and do not forget to do some recursion to get all of these nasty submenus
    for (int i = 0; i < subMenu.count(); i++)
    {
        subMenu.at(i)->fillQMenu(newCurrent,_list);
    }
}



/**
 * @brief puts some info out on qDebug();
 * Puts name, submenus and if the crosspoint has includes out on qDebug()
 */
void XdgMenuCross::debugCross()
{
    qDebug() << "Name: " << name;
    if (hasInclude)
        qDebug() << "Include-Tree!";
    for (int i=0; i < subMenu.count(); i++)
    {
        qDebug()<<"("<<name<<")"<<"\n"<<"|-";
        subMenu.at(i)->debugCross();
    }
}
/**
 * @brief The main function of this class
 * Checks if _item fits on this menu-crosspoint or any of its children
 * @param[in] _item a pointer to an Xdgdesktopfile you want to fit in the menu-tree
 */
bool XdgMenuCross::fitsHere(XdgDesktopFile* _item)
{
    //check for fittage in sub-menus for the whole tree
    for (int i=0; i < subMenu.count(); i++)
        subMenu.at(i)->fitsHere(_item);

    //then check this one
    if (hasInclude && _item->useable==true && !usedNames.contains(_item->getValue("Name")) )
    {
        if (inc_categories->validate(_item))
        {
            //every item only once
            _item->useable=false;
            //every "name" only once! (had like 12 okulars in my list! (I HATE KDE!)
            usedNames.push_back(_item->getValue("Name"));

            entries.push_back(_item);

#ifndef QNODEBUGCROSS
            qDebug() << "Xdgmenucross: This fits here! (" << name << directory << ")";
#endif
            return true;
        }
        else
        {
#ifndef QNODEBUGCROSS

            qDebug() << "Xdgmenucross: does not fit here! (" << name << directory << ")";
#endif
            return false;
        }
    }
    else

#ifndef QNODEBUGCROSS
        qDebug() << "Xdgmenu: " << name << "had no includes or already used item!";
#endif
    return false;

}


/**
 * @brief The constructor of the class Xdgmenulogic
 * This is given 3 parameters to construct a new menulogic-item
 * @param[in] _logic The type of the logic, currently there is XDMENULOGIC_AND and XDGMENULOGIC_OR
 * @param[in] _not if you want to use the Xdgmenulogic-result to be reversed
 * @param[in] _parent if this should be a child of another Xdgmenulogic
 */


XdgMenuLogic::XdgMenuLogic(int _logic, bool _not, XdgMenuLogic* _parent)
{
    qDebug() << "Xdgmenulogic: initialising...";
    logic = _logic;
    not_ = _not;
    parent = _parent;
}



/**
 * @brief The destructor of the class Xdgmenulogic
 * @todo Do This!
 */
XdgMenuLogic::~XdgMenuLogic()
{
    qDebug() << "Xdgmenulogic: initialising... bad";
}


/**
 * @brief The constructor-stub of Xdgmenulogic
 * This is a stub for making this class QList compatible!
 */
XdgMenuLogic::XdgMenuLogic()
{
}


/**
 * @brief This adds a new Xdgmenulogic to the childlist
 * @param[in] _logic The type of the logic, currently there is XDMENULOGIC_AND and XDGMENULOGIC_OR
 * @param[in] _not if you want to use the Xdgmenulogic-result to be reversed
 * @param[in] _parent if this should be a child of another Xdgmenulogic
 * @return the childindex + 1.. so if you want to use the new child use the return - 1!
 *
 */

int XdgMenuLogic::makeChild(int _logic, bool _not, XdgMenuLogic* _parent)
{
    qDebug() << "Xdgmenulogic: making Child";
    XdgMenuLogic* pushit = new XdgMenuLogic(_logic, _not, _parent);
    pushit->hasParent=true;
    children.push_back(pushit);
    parent = _parent;
    return children.count();
}
/**
 * @brief This is the main function of the Xdgmenulogic
 * It checks if a given _item validates with this logic or not
 * @param[in] _item the Xdgdesktopfile* you want to validate
 * @return if the Logic validates the _item as fitting the logic or not fitting!
 *
 */


bool XdgMenuLogic::validate(XdgDesktopFile* _item)
{
#ifndef QNODEBUGCROSS
    if (cat_list.count() > 0)
        qDebug() << cat_list.at(0);
#endif
    //recursion first! then do actual work.. this is mixed here to save one if ;)
    if (logic == XDGMENULOGIC_AND)
    {
        for (int i=0; i < children.count(); i++)
        {
            if (children.at(i)->validate(_item)==false)
            {
#ifndef QNODEBUGCROSS
                qDebug()<< "Xdgmenulogic: AND validation negative, children said no!";
#endif
                return not_;

            }
        }

        for (int i=0; i < _item->categories.count(); i++)
        {
            if (cat_list.contains(_item->categories.at(i))==true)
            {
#ifndef QNODEBUGCROSS
                qDebug() << "Xdgmenulogic: AND validation result positive... returning: " << !not_;
#endif
                return !not_;
            }
        }
        if (cat_list.count() == 0)
        {
#ifndef QNODEBUGCROSS
            qDebug() << "Xdgmenulogic: beeing a logic parent! (not: "<<not_ <<"logic: "<< logic<<") returning: " << !not_;
#endif
            return !not_;
        }
        else
        {
#ifndef QNODEBUGCROSS
            qDebug() << "Xdgmenulogic: AND validation result negative, i said no!";
#endif
            return not_;
        }
    }
    else if (logic == XDGMENULOGIC_OR)
    {
        for (int i=0; i < children.count(); i++)
        {
            if (children.at(i)->validate(_item)==true)
                return !not_;
        }

        for (int i=0; i < _item->categories.count(); i++)
        {
            if (cat_list.contains(_item->categories.at(i))==true)
                return !not_;
        }

        return not_;
    }


    return false; // will not happen anyway, just for the compiler-warnings..

}



#endif
