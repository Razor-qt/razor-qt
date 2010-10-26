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


void Razordeskmanagerlegacy::getFileList()
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
            Xdgdesktopfile* tmp = new Xdgdesktopfile(desktop_file_iter.filePath());
            tmp->parseFile();

            if (tmp->getValue("Hidden")=="true" || tmp->getValue("NotShowIn")=="razor" ||
                    ((tmp->getValue("OnlyShowIn") != "") && !tmp->getValue("OnlyShowIn").contains("razor")) )  
	      badNames.push_back(tmp->getValue("Name")); //this is needed as std says all with the same name get removed by one entry with these flags too
            else
                itemList.append(tmp);
        }
    }
    
    qDebug() << "badnames: " << badNames;
    qDebug() << "itemlist: " << itemList;
    
    
    //purge the badones
    for (int i=0; i < itemList.count(); i++)
    {
      if (badNames.contains(itemList.at(i)->getValue("Name")))
      {
	qDebug() << "Deleting item!";
	delete itemList.at(i);
	itemList.removeAt(i);
      }
    }
    qDebug() << "Razordeskmanl: found " << itemList.count() << " usable desktop-entries";
    
}

void Razordeskmanagerlegacy::updateIconList()
{
  getFileList();
}


QList< Razordeskicondata* >* Razordeskmanagerlegacy::iconList()
{
    return privIconList();
}


// create the desktop icons and link them with action!
void Razordeskmanagerlegacy::showIcons()
{
  int x,y;
  x=0;
  y=30;
  //Xfitman iconplacer;
  for (int i = 0; i < itemList.count() ; i++)
  {
    QPoint pos(x,y);
    Razordeskicondata* tmp = new Razordeskicondata(Razor::getInstance().geticontheme()->getIconNG(itemList.at(i)->getValue("Icon")),
						   itemList.at(i)->getValue("Name"),
						   itemList.at(i)->getValue("Comment"),
						   pos, workSpace());
    tmp->setData(itemList.at(i)->getValue("Exec"));
    connect(tmp, SIGNAL(moved(QPoint)), this, SLOT(saveIconState()));
     //iconplacer.moveWindowtoDesktop(tmp->getWin(),-1);
        y += 100;
	privIconList()->append(tmp);
  }
}

Razordeskmanagerlegacy::~Razordeskmanagerlegacy()
{
}

Razordeskmanagerlegacy::Razordeskmanagerlegacy(Razorworkspace* _workspace) : Razordeskmanager(_workspace)
{
  updateIconList();
  showIcons();
  restoreIconState();
  qDebug() << "initialisation done";
}



#endif
