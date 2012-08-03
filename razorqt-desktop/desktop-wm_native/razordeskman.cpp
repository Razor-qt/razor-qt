/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include <QApplication>
#include <QString>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDirIterator>
#include <QVariant>
#include <QX11Info>
#include <QtDebug>

#include <X11/Xatom.h>

#include <razorqt/razorsettings.h>
#include "razorqt/xfitman.h"

#include "razordeskman.h"


EXPORT_RAZOR_DESKTOP_PLUGIN_CPP(RazorDeskManager);


QString RazorDeskManager::info()
{
    return tr("A legacy desktop manager. "
              "It allows to use window manager's native menus. "
              "Only classical icons and simple wallpaper functionality is implemented"
            );
}

void RazorDeskManager::saveIconState()
{
    qDebug() << "saveIconState" << m_iconList;
    deskicons->beginWriteArray("icons");
    IconMapIterator i(m_iconList);
    int ix = 0;
    while (i.hasNext())
    {
        i.next();
        deskicons->setArrayIndex(ix);
        deskicons->setValue("name", i.key());
        deskicons->setValue("point", i.value()->pos());
        ++ix;
    }
    deskicons->endArray();
}


RazorDeskManager::~RazorDeskManager()
{
    IconMapIterator i(m_iconList);
    while (i.hasNext())
    {
        i.next();
        delete i.value();
    }
    m_iconList.clear();
}


void RazorDeskManager::restoreIconState()
{
    qDebug() << "restoring icon state!";

    int count = deskicons->beginReadArray("icons");

    // Store positions of already setPos-ed icons here temporarily - used in calculation below.
    QList<QRect> positionFrames;

    // First - try to move user moved icons to saved location
    QString name;
    for (int i = 0; i < count; ++i)
    {
        deskicons->setArrayIndex(i);
        name = deskicons->value("name", "").toString();
        if (! m_iconList.contains(name))
        {
            continue;
        }

        RazorDeskIconBase* icon = m_iconList[name];
        QPoint p = deskicons->value("point").value<QPoint>();
        icon->setPos(p);
        positionFrames.append(QRect(p.x(), p.y(), icon->width(), icon->height()));
    }
    deskicons->endArray();
    
    // Then take all remaining icons (QPoint(0.0)) and try to calculate
    // their position.
    
    IconMapIterator it(m_iconList);
    QDesktopWidget *dw = QApplication::desktop();
    int xMax = dw->availableGeometry().width() - 140;
    int yMax = dw->availableGeometry().height();
    // initial placement to be tried
    int x = 10;
    int y = 10;
    // those 70's are taken from RazorDeskIconBase:paintEvent. Ugly, but it's fine for now.
    int w = 70;
    int h = 70;
    while (it.hasNext())
    {
        it.next();
        // it's moved already to its saved position
        if (it.value()->pos() != QPoint(0,0))
            continue;
        // 
        QRect newPos(x, y, w, h);
        foreach (QRect r, positionFrames)
        {
            if (!r.intersects(newPos))
                continue;

            y += 80;
            if (y > yMax)
            {
                y = 10;
                x += 80;
                if (x > xMax)
                {
                    // TODO/FIXME: better failsafe...
                    it.value()->setPos(QPoint(1, 1));
                    qDebug() << "Position boundaries reached. Ite means there is no free space on screen. Saving at 1,1...";
                }
            }
            newPos = QRect(x, y, w, h);
        } // foreach positionFrames
        
        //qDebug() << "Position from calculation (free space)" << newPos;
        positionFrames.append(newPos);
        it.value()->setPos(newPos.topLeft());

    } // while it

    qDebug() << "restoring done";
}



RazorDeskManager::RazorDeskManager(const QString & configId, RazorSettings * config)
    : DesktopPlugin(configId, config)
{
    m_launchMode = DesktopPlugin::launchModeFromString(config->value("icon-launch-mode").toString());

    config->beginGroup(configId);
    bool makeIcons = config->value("icons", false).toBool();
    //now we got the desktop we need to determine if the user wants a defined picture there
    QString finalPixmap = config->value("wallpaper", "").toString();
    config->endGroup();

    if (finalPixmap.isEmpty() || !QFile::exists(finalPixmap))
    {
        //now we want to use the system default - we still need to find that one out though
        finalPixmap = razorTheme.desktopBackground();
        qDebug() << "trying to get system-defaults" << finalPixmap;
    }

    if (! finalPixmap.isEmpty())
    {
        qDebug() << "Creating wallpaper";
        int width,height;
        QDesktopWidget * dw = QApplication::desktop();
        if (dw->screenCount() == 1)
        {
            width=dw->width();
            height = dw->height();
        }
        else
        {
            width=dw->screenGeometry(-1).width();
            height=dw->screenGeometry(-1).height();
        }

        QPixmap pixmap(finalPixmap);
        pixmap = pixmap.scaled(width,height);
        Pixmap p = pixmap.handle();
        XGrabServer(QX11Info::display());
        XChangeProperty(QX11Info::display(), QX11Info::appRootWindow(), XfitMan::atom("_XROOTPMAP_ID"), XA_PIXMAP, 32, PropModeReplace, (unsigned char *) &p, 1);
        XChangeProperty(QX11Info::display(), QX11Info::appRootWindow(), XfitMan::atom("ESETROOT_PMAP_ID"), XA_PIXMAP, 32, PropModeReplace, (unsigned char *) &p, 1);
        XSetCloseDownMode(QX11Info::display(), RetainPermanent);
        XSetWindowBackgroundPixmap(QX11Info::display(), QX11Info::appRootWindow(), p);
        XClearWindow(QX11Info::display(), QX11Info::appRootWindow());
        XUngrabServer(QX11Info::display());
        XFlush(QX11Info::display());
    }
    
    if (makeIcons)
    {
        deskicons = new RazorSettings("deskicons", this);    
        m_fsw = new QFileSystemWatcher(QStringList() << QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), this);
        connect(m_fsw, SIGNAL(directoryChanged(const QString&)), this, SLOT(updateIconList()));
        updateIconList();
    }
}

void RazorDeskManager::updateIconList()
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
            XdgDesktopFile* tmp = new XdgDesktopFile();
            tmp->load(df);

            if (tmp->isShow())
            {
                idata = new RazorDeskIconDesktop(tmp, pos);
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
            idata = new RazorDeskIconFile(df, pos);
        }
        
        idata->setLaunchMode(m_launchMode);

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
