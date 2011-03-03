#ifndef RAZORDESKMANL_CPP
#define RAZORDESKMANL_CPP

#include <QDesktopServices>

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

    QDirIterator dirIter(QDesktopServices::storageLocation(QDesktopServices::DesktopLocation),
                         //QDir::NoDotAndDotDot,
                         QDirIterator::Subdirectories);

    int x = 0;
    int y = 30;
    QStringList tmpList;

    while (dirIter.hasNext())
    {
        dirIter.next();
        qDebug() << dirIter.filePath();

        if (dirIter.filePath().endsWith(".desktop")) //only use .desktop files!
        {
            QString df(dirIter.filePath());
            tmpList.append(df);
            // only non existing icons are created
            if (m_iconList.contains(df))
            {
                qDebug() << "updateIconList REREAD. Skip:" << df;
                continue;
            }
            
            XdgDesktopFile* tmp = new XdgDesktopFile(df);

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

                m_iconList[df] = idata;

                // HACK: there should be better algorithm for this.
                // and it does not count with panels...
                y += 70;
                if (y > maxHeight-60)
                {
                    y = 30;
                    x += 90;
                }

            }
            delete tmp;
        }
    }

    // now remove potentialy deleted icons
    IconMapIterator iter(m_iconList);
    while (iter.hasNext())
    {
        iter.next();
        if (tmpList.contains(iter.key()))
            continue;
        delete m_iconList.take(iter.key());
    }

    qDebug() << "Razordeskmanl: found " << m_iconList.count() << " usable desktop-entries";
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
