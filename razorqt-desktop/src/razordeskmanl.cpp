#ifndef RAZORDESKMANL_CPP
#define RAZORDESKMANL_CPP

#include "razordeskmanl.h"
#include "razor.h"
#include <razorqt/xfitman.h>
#include <razorqt/xdgiconthemefile.h>
/**
 * @file razordeskmanl.cpp
 * @author Christopher "VdoP" Regali
 * @brief implements Razordeskmanagerlegacy class
 */


void RazorDeskManagerLegacy::getFileList()
{
    itemList.clear();
    QList<QString> badNames;
    QDirIterator desktop_file_iter(QDir::homePath()+"/Desktop/", QDirIterator::Subdirectories);
    int i = 0;
    while (desktop_file_iter.hasNext())
    {
        i++;
        desktop_file_iter.next();
        if (desktop_file_iter.fileName() == "." || desktop_file_iter.fileName() == "..")
            continue;
        if (desktop_file_iter.filePath().endsWith(".desktop")) //only use .desktop files!
        {
            XdgDesktopFile* tmp = new XdgDesktopFile(desktop_file_iter.filePath());

            if (tmp->isShow())
                itemList.append(tmp);
            else
                badNames.push_back(tmp->value("Name").toString()); //this is needed as std says all with the same name get removed by one entry with these flags too
        }
    }

    qDebug() << "badnames: " << badNames;
    //qDebug() << "itemlist: " << itemList;


    //purge the badones
    for (int i=0; i < itemList.count(); i++)
    {
        if (badNames.contains(itemList.at(i)->value("Name").toString()))
        {
            qDebug() << "Deleting item!";
            delete itemList.at(i);
            itemList.removeAt(i);
        }
    }
    qDebug() << "Razordeskmanl: found " << itemList.count() << " usable desktop-entries";

}

void RazorDeskManagerLegacy::updateIconList()
{
    getFileList();
}


QList< RazorDeskIconData* >* RazorDeskManagerLegacy::iconList()
{
    return privIconList();
}


// create the desktop icons and link them with action!
void RazorDeskManagerLegacy::showIcons()
{
    int x,y;
    x=0;
    y=30;
    //Xfitman iconplacer;
    for (int i = 0; i < itemList.count() ; i++)
    {
        QPoint pos(x,y);
        RazorDeskIconData* tmp = new RazorDeskIconData(Razor::getInstance().geticontheme()->getIconNG(itemList.at(i)->value("Icon").toString()),
                itemList.at(i)->value("Name").toString(),
                itemList.at(i)->value("Comment").toString(),
                pos, workSpace());
        tmp->setData(itemList.at(i)->value("Exec").toString());
        connect(tmp, SIGNAL(moved(QPoint)), this, SLOT(saveIconState()));
        //iconplacer.moveWindowtoDesktop(tmp->getWin(),-1);
        y += 100;
        privIconList()->append(tmp);
    }
}

RazorDeskManagerLegacy::~RazorDeskManagerLegacy()
{
}

RazorDeskManagerLegacy::RazorDeskManagerLegacy(RazorWorkSpace* _workspace) : RazorDeskManager(_workspace)
{
    updateIconList();
    showIcons();
    restoreIconState();
    qDebug() << "initialisation done";
}



#endif
