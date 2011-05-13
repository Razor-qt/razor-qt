/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "iconview.h"
#include "iconscene.h"
#include <QtDebug>
#include <QDesktopServices>
#include <QGraphicsView>
#include <QPropertyAnimation>
#include <QFileDialog>


EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_CPP(IconView)


IconView::IconView(QGraphicsScene * scene, const QString & configId, ReadSettings * config)
    : DesktopWidgetPlugin(scene, configId, config)
{
    setObjectName("IconView");

    QSettings * s = config->settings();
    s->beginGroup(configId);

    QString dir = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
    dir = s->value("directory", dir).toString();
    s->endGroup();

    // Hack to ensure the fully transparent QGraphicsView background
    QPalette palette;
    palette.setBrush(QPalette::Base, Qt::NoBrush);
    setPalette(palette);
    // Required to display wallpaper
    setAttribute(Qt::WA_TranslucentBackground);
    // no border at all finally
    setFrameShape(QFrame::NoFrame);
    
    m_scene = new IconScene(dir);
   
    setScene(m_scene);
    
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setRenderHint(QPainter::HighQualityAntialiasing);
    
    setDragMode(QGraphicsView::RubberBandDrag);
    
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    
    setCacheMode(QGraphicsView::CacheBackground);
    
    setAlignment(Qt::AlignTop | Qt::AlignLeft);
    
    // "cool" display effect
    setWindowOpacity(0.0);
}

IconView::~IconView()
{
    delete m_scene;
}

    
QString IconView::info()
{
    return QObject::tr("Display content of the given directory/folder");
}

QString IconView::instanceInfo()
{
    return QObject::tr("Icon View:") + " " + m_configId;
}

void IconView::setSizeAndPosition(const QPointF & position, const QSizeF & size)
{
    qDebug() << "Moving to" << position << "resizing" << size;
    move(position.x(), position.y());
    resize(size.width(), size.height());
    m_scene->setParentSize(size);
    
    // "cool" display FX - the main part
    QPropertyAnimation * animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(500);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start();
}

void IconView::save()
{
    QSettings *s = m_config->settings();
    s->beginGroup(m_configId);
    s->setValue("plugin", "iconview");
    s->setValue("x", pos().x());
    s->setValue("y", pos().y());
    s->setValue("w", size().width());
    s->setValue("h", size().height());
    s->setValue("directory", m_scene->dir());
    s->endGroup();
}

void IconView::configure()
{
    QString txt = QFileDialog::getExistingDirectory(0, tr("Icon View Configuration"),
                                                    m_scene->dir());
    if (txt.isNull())
        return;

    m_scene->setDir(txt);
    save();
}

bool IconView::blockGlobalMenu()
{
    return m_scene->blockGlobalMenu();
}
