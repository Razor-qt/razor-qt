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


void RazorDeskManagerLegacy::updateIconList()
{
    int maxHeight = QApplication::desktop()->screenGeometry().height();
    privIconList.clear();
    QDirIterator dirIter(QDir::homePath()+"/Desktop/",
                         //QDir::NoDotAndDotDot,
                         QDirIterator::Subdirectories);

    int x = 0;
    int y = 30;
    while (dirIter.hasNext())
    {
        dirIter.next();
        qDebug() << dirIter.filePath();

        if (dirIter.filePath().endsWith(".desktop")) //only use .desktop files!
        {
            XdgDesktopFile* tmp = new XdgDesktopFile(dirIter.filePath());

            if (tmp->isShow())
            {
                qDebug() << "ISSHOW";
                QPoint pos(x, y);
                RazorDeskIcon* idata = new RazorDeskIcon(
                            tmp->value("Exec").toString(),
                            Razor::getInstance().geticontheme()->getIconNG(tmp->value("Icon").toString()),
                            tmp->value("Name").toString(),
                            tmp->value("Comment").toString(),
                            pos,
                            workSpace()
                        );
                connect(idata, SIGNAL(moved(QPoint)), this, SLOT(saveIconState()));
                privIconList.append(idata);

                // HACK: there should be better algorithm for this.
                // and it does not count with panels...
                y += 100;
                if (y > maxHeight-60)
                {
                    y = 30;
                    x += 90;
                }

            }
            delete tmp;
        }
    }

    qDebug() << "Razordeskmanl: found " << privIconList.count() << " usable desktop-entries";
}


QList< RazorDeskIcon* > RazorDeskManagerLegacy::iconList()
{
    return privIconList;
}

RazorDeskManagerLegacy::~RazorDeskManagerLegacy()
{
}

RazorDeskManagerLegacy::RazorDeskManagerLegacy(RazorWorkSpace* _workspace) : RazorDeskManager(_workspace)
{
    updateIconList();
    restoreIconState();
    qDebug() << "initialisation done";
}



#endif
