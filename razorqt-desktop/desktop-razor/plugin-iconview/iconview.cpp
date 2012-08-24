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

#include <QtCore/QtDebug>
#include <QtGui/QDesktopServices>
#include <QtGui/QGraphicsView>
#include <QtCore/QPropertyAnimation>
#include <QtGui/QFileDialog>

#include "iconview.h"
#include "iconscene.h"

EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_CPP(IconView)


IconView::IconView(DesktopScene * scene, const QString & configId, RazorSettings * config)
    : DesktopWidgetPlugin(scene, configId, config)
{
    m_proxy = new QGraphicsProxyWidget(this);
    m_widget = new IconViewWidget(configId, config);
    m_widget->show();
    m_proxy->setWidget(m_widget);
    m_proxy->show();
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
    DesktopWidgetPlugin::setSizeAndPosition(position, size);
    m_widget->setSizeAndPosition(position, size);
}

void IconView::save()
{
    m_config->beginGroup(m_configId);
    m_config->setValue("plugin", "iconview");
    m_config->setValue("x", pos().x());
    m_config->setValue("y", pos().y());
    m_config->setValue("w", m_boundingRect.width());
    m_config->setValue("h", m_boundingRect.height());
    m_config->setValue("directory", m_widget->dir());
    m_config->endGroup();
}

void IconView::configure()
{
    QString txt = QFileDialog::getExistingDirectory(0, tr("Icon View Configuration"),
                                                    m_widget->dir());
    if (txt.isNull())
        return;

    m_widget->setDir(txt);
    save();
}



IconViewWidget::IconViewWidget(const QString & configId, RazorSettings * config)
{
    setObjectName("IconView");

    config->beginGroup(configId);

    QString dir = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
    dir = config->value("directory", dir).toString();
    config->endGroup();

    // Hack to ensure the fully transparent QGraphicsView background
    QPalette palette;
    palette.setBrush(QPalette::Base, Qt::NoBrush);
    setPalette(palette);
    // Required to display wallpaper
    setAttribute(Qt::WA_TranslucentBackground);
    // no border at all finally
    setFrameShape(QFrame::NoFrame);
    
    setAcceptDrops(true);
    
    m_iconScene = new IconScene(dir);
   
    setScene(m_iconScene);
    
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setRenderHint(QPainter::HighQualityAntialiasing);
    
    setDragMode(QGraphicsView::RubberBandDrag);
    
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    
    setCacheMode(QGraphicsView::CacheBackground);
    
    setAlignment(Qt::AlignTop | Qt::AlignLeft);
}

IconViewWidget::~IconViewWidget()
{
    delete m_iconScene;
}

void IconViewWidget::setSizeAndPosition(const QPointF & position, const QSizeF & size)
{
    m_iconScene->setParentSize(size);
    resize(size.width(), size.height());
}

QString IconViewWidget::dir()
{
    return m_iconScene->dir();
}

void IconViewWidget::setDir(const QString &txt)
{
    m_iconScene->setDir(txt);
}

