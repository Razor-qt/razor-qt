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
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QMessageBox>
#include <QtGui/QDropEvent>
#include <QtDebug>

#include "razorworkspace.h"
#include "backgroundprovider.h"
#include "desktopscene.h"


RazorWorkSpace::RazorWorkSpace(DesktopScene *scene, int screen, int desktop, QWidget* parent)
    : QGraphicsView(parent),
      m_scene(scene),
      m_screen(screen),
      m_desktop(desktop),
      m_backgroundItem(0)
{
    //qDebug() << "RazorWorkSpace::RazorWorkSpace";
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);
    setAttribute(Qt::WA_X11NetWmWindowTypeDesktop);
    setFrameShape(QFrame::NoFrame);
    setWindowTitle(QString("Razor Desktop %1").arg(screen));
    setAcceptDrops(true);
    
    m_background = new BackgroundProvider(m_screen, this);

    //qDebug() << "File Name:" << m_menuFile;

    // this is mandatory for virtualized (virtualbox) installations. Dunno why.
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(QApplication::desktop(), SIGNAL(workAreaResized(int)),
            this, SLOT(workspaceResized(int)));
    
    setScene(m_scene);
    
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    //setRenderHint(QPainter::HighQualityAntialiasing);
    
    setDragMode(QGraphicsView::RubberBandDrag);
    
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    
    setCacheMode(QGraphicsView::CacheBackground);
}

void RazorWorkSpace::setConfig(const WorkspaceConfig & bg)
{
    switch (bg.wallpaperType)
    {
        case (RazorWorkSpaceManager::BackgroundPixmap):
        {
            QPixmap pm(bg.wallpaper);
            if (pm.isNull())
                // TODO/FIXME: defaults
                qDebug() << "Pixmap is null" << bg.wallpaper;

            m_background->setPixmap(pm);
            m_background->setScaleRatio(bg.keepAspectRatio ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio);
            break;
        }
        default:
        {
            m_background->setColor(QColor(bg.wallpaper));
        }
    }
    
    workspaceResized(m_screen);
}

void RazorWorkSpace::saveConfig()
{
    DesktopConfig::instance()->config->beginGroup("razor");
    DesktopConfig::instance()->config->beginWriteArray("screens");
    DesktopConfig::instance()->config->setArrayIndex(m_screen);
    DesktopConfig::instance()->config->beginWriteArray("desktops");
    DesktopConfig::instance()->config->setArrayIndex(m_desktop);
    DesktopConfig::instance()->config->endArray();
    DesktopConfig::instance()->config->endArray();
    DesktopConfig::instance()->config->endGroup();
}


void RazorWorkSpace::workspaceResized(int screen)
{
    if (screen != m_screen)
        return;

    QRect geometry(QApplication::desktop()->screenGeometry(screen)); 
    move(geometry.x(), geometry.y());
    resize(geometry.width(), geometry.height());

    if (!m_backgroundItem)
    {
        m_backgroundItem = new QGraphicsPixmapItem();
        m_scene->addItem(m_backgroundItem);
    }
    m_backgroundItem->setPixmap(m_background->pixmap(geometry));
    m_backgroundItem->setPos(geometry.x(), geometry.y());

    setSceneRect(geometry);
}

void RazorWorkSpace::setDesktopBackground()
{
	if (m_background->gui())
		workspaceResized(m_screen);
}

RazorWorkSpace::~RazorWorkSpace()
{
}

void RazorWorkSpace::dragEnterEvent(QDragEnterEvent *event)
{
    if (!event)
        return;

    if (event->mimeData()->urls().count() == 0)
    	return;

    QString fileToDrop = event->mimeData()->urls().value(0).toLocalFile();
    if (QPixmap(fileToDrop).isNull())
    	return;

    event->accept();
}

void RazorWorkSpace::dragMoveEvent(QDragMoveEvent * event)
{
    if (!event)
        return;
    event->accept();
}

void RazorWorkSpace::dropEvent(QDropEvent *event)
{
	qDebug() << 1;
    if (!event)
        return;

    if (event->mimeData()->urls().count() == 0)
        return;

    QString fileToDrop = event->mimeData()->urls().value(0).toLocalFile();
    // here we can do some fancy stuff for Directories/folders, files etc.
    // But just change background for now...
    if (QMessageBox::question(this, tr("Background Change"),
                           tr("Do you want to change desktop background?"),
                           QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
    {
        return;
    }

    m_background->setFile(fileToDrop);
    workspaceResized(m_screen);
    event->acceptProposedAction();
}
