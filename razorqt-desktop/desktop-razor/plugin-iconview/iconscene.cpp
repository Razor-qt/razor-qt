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

#include <QtCore/QUrl>
#include <QtCore/QDir>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QFileInfoList>
#include <QtDebug>
#include <QtGui/QDesktopServices>
#include <QtGui/QMessageBox>

#include <razorqt/razorsettings.h>
#include "iconscene.h"
#include "desktopicon.h"


IconScene::IconScene(const QString & directory, QObject * parent)
    : QGraphicsScene(parent),
      m_fsw(0)
{
    setDirImpl(directory);
    
    RazorSettings s("desktop");
    m_launchMode = DesktopPlugin::launchModeFromString(s.value("icon-launch-mode").toString());
}

void IconScene::setDir(const QString & directory)
{
    setDirImpl(directory, true);
}

void IconScene::setDirImpl(const QString & directory, bool repaint)
{
    if (!directory.isEmpty() && m_directory == directory)
        return;

    if (m_fsw)
    {
        delete m_fsw;
        m_fsw = 0;
    }

    QStringList dirs;
    if (!directory.isEmpty() && QDir(directory).exists())
    {
        qDebug() << "Setting dir from prefs" << directory;
        m_directory = directory;
    }
    else if (QDir(QDesktopServices::storageLocation(QDesktopServices::DesktopLocation)).exists())
    {
        qDebug() << "Setting dir" << "DesktopLocation";
        m_directory = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
    }
    else if (QDir(QDir::homePath()).exists())
    {
        qDebug() << "Setting dir" << "homePath";
        m_directory = QDir::homePath();
    }
    else
    {
        qDebug() << "Setting dir" << "filesystem root";
        m_directory = QDir::rootPath();
    }

    if (repaint)
        updateIconList();

    m_fsw = new QFileSystemWatcher(QStringList() << m_directory, this);
    connect(m_fsw, SIGNAL(directoryChanged(const QString&)), this, SLOT(updateIconList()));
}

void IconScene::dragEnterEvent(QGraphicsSceneDragDropEvent *e)
{
    qDebug() << "IconScene::dragEnterEvent" << e->mimeData()->hasUrls();
    // Getting URL from mainmenu...
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void IconScene::dragMoveEvent(QGraphicsSceneDragDropEvent *e)
{
    dragEnterEvent(e);
}

void IconScene::dropEvent(QGraphicsSceneDragDropEvent *e)
{
    qDebug() << "IconScene::dropEvent" << e->mimeData()->urls();
    const QMimeData *mime = e->mimeData();
    // urls from mainmenu
    foreach (QUrl url, mime->urls())
    {
        QFileInfo fi(url.toLocalFile());
        QFile f(url.toLocalFile());
        if (!f.copy(m_directory + "/" + fi.fileName()))
        {
            QMessageBox::information(0, tr("Copy File Error"),
                                     tr("Cannot copy file %1 to %2").arg(url.toLocalFile()).arg(m_directory));
        }
    }
}

void IconScene::updateIconList()
{
    qDebug() << "IconScene::updateIconList";
  
    if (m_fsw) m_fsw->blockSignals(true);

    // bruteforce cleanup
    foreach (QGraphicsItem* item, items())
    {
        removeItem(item);
        delete item;
    }
    
    //QDirIterator dirIter(m_fsw->directories().at(0));
    QDir d(m_directory);
    int x = 30;
    int y = 10;

    //while (dirIter.hasNext())
    foreach (QFileInfo dirIter, d.entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries, QDir::DirsFirst|QDir::Name|QDir::IgnoreCase ))
    {
//        dirIter.next();
        QString df(dirIter.filePath());

        // HACK: QDir::NoDotAndDotDot does not work so this fixes it...
        if (df.endsWith("/..") || df.endsWith("/."))
            continue;

        //qDebug() << df;
        IconBase * idata = 0;

        if (dirIter.filePath().endsWith(".desktop")) //only use .desktop files!
        {
            XdgDesktopFile* tmp = new XdgDesktopFile();
            tmp->load(df);

            if (tmp->isShow())
            {
                idata = new DesktopIcon(tmp);
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
            idata = new FileIcon(df);
        }

        if (idata)
        {
            //qDebug() << "   POSITIONING" << x << y;
            addItem(idata);
            idata->setPos(x, y);
            idata->setLaunchMode(m_launchMode);
            while (idata->collidingItems().count())
            {
                idata->setPos(x, y*IconBase::iconHeight());
                y += IconBase::iconHeight() + 20;
            }
            y += IconBase::iconHeight() + 20;
            
            if (y > m_parentSize.height() - IconBase::iconHeight()) //IconBase::iconHeight() simulates the slider. Ugly, I know...
            {
                x = x + IconBase::iconWidth() + 20;
                y = 10;
            }
        }
    }

    if (m_fsw) m_fsw->blockSignals(false);
}

void IconScene::setParentSize(const QSizeF & size)
{
    qDebug() << "IconScene::setParentSize" << m_parentSize << size;
    if (m_parentSize != size)
    {
        m_parentSize = size;
        updateIconList();
    }
}

