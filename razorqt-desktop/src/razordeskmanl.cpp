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
    m_fsw->blockSignals(true);

    qDebug() << "updateIconList";
    QDirIterator dirIter(QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));

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

        QPoint pos(0, 0);
        RazorDeskIconBase * idata;

        if (dirIter.filePath().endsWith(".desktop")) //only use .desktop files!
        {
            XdgDesktopFile* tmp = new XdgDesktopFile(df);

            if (tmp->isShow())
            {
                idata = new RazorDeskIconDesktop(tmp, pos, workSpace());
            }
            else
            {
                delete tmp;
                qDebug() << "Desktop file" << df << "isShow==false";
                continue;
            }
        }
        else
        {
            idata = new RazorDeskIconFile(df, pos, workSpace());
        }

        connect(idata, SIGNAL(moved(QPoint)), this, SLOT(saveIconState()));
        m_iconList[df] = idata;
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

    restoreIconState();
    m_fsw->blockSignals(false);
}

RazorDeskManagerLegacy::~RazorDeskManagerLegacy()
{
}

RazorDeskManagerLegacy::RazorDeskManagerLegacy(RazorWorkSpace* _workspace) : RazorDeskManager(_workspace)
{
    updateIconList();
    qDebug() << "initialisation done";
}



#endif
