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
    qDebug() << "updateIconList";
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
        QString df(dirIter.filePath());
        
        // HACK: QDir::NoDotAndDotDot does not work so this fixes it...
        if (df.endsWith("/..") || df.endsWith("/."))
            continue;
        
        qDebug() << df;
        tmpList.append(df);
        
        // only non existing icons are created
        if (m_iconList.contains(df))
        {
            qDebug() << "updateIconList REREAD. Skip:" << df;
            continue;
        }
        
        QPoint pos(x, y);
        RazorDeskIconBase * idata;

        if (dirIter.filePath().endsWith(".desktop")) //only use .desktop files!
        {
            XdgDesktopFile* tmp = new XdgDesktopFile(df);

            if (tmp->isShow())
            {
                idata = new RazorDeskIconDesktop(tmp, pos, workSpace());
            }
            else
                delete tmp;
        }
        else
        {
            idata = new RazorDeskIconFile(df, pos, workSpace());
        }

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
