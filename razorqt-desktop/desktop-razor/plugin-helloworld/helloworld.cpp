/* BEGIN_COMMON_COPYRIGHT_HEADER
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
 * version 3 of the License, or (at your option) any later version.
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

#include "helloworld.h"
#include <QtDebug>
#include <QGraphicsScene>
#include <QInputDialog>


EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_CPP(HelloWorld)


HelloWorld::HelloWorld(QGraphicsScene * scene, const QString & configId, RazorSettings * config)
    : DesktopWidgetPlugin(scene, configId, config)
{
    m_config->beginGroup(configId);
    
    setOpenExternalLinks(true);
    QString text(m_config->value("text", "Lorem Ipsum").toString());
    setHtml(text);
    
    QString color(m_config->value("color", "").toString());
    if (! color.isEmpty())
        setDefaultTextColor(QColor(color));

    QFont f = font();
    f.setPixelSize(48);
    setFont(f);
    
    m_config->endGroup();
}

HelloWorld::~HelloWorld()
{
}

    
QString HelloWorld::info()
{
    return QObject::tr("Display simple text. A debugging/sample widget.");
}

QString HelloWorld::instanceInfo()
{
    return tr("Hello World:") + " " + m_configId;
}

void HelloWorld::setSizeAndPosition(const QPointF & position, const QSizeF & size)
{
    qDebug() << "Moving to" << position;
    setPos(position);
}

void HelloWorld::configure()
{
    bool ok;
    QString txt = QInputDialog::getText(0, tr("Display Text Configuretion"), tr("Edit HTML"), QLineEdit::Normal, toHtml(), &ok);
    if (!ok)
        return;
    setHtml(txt);
    save();
}

void HelloWorld::save()
{
    m_config->beginGroup(m_configId);
    m_config->setValue("plugin", "helloworld");
    m_config->setValue("x", pos().x());
    m_config->setValue("y", pos().y());
    m_config->setValue("w", boundingRect().width());
    m_config->setValue("h", boundingRect().height());
    m_config->setValue("text", toHtml());
    m_config->setValue("color", defaultTextColor().name());
    m_config->endGroup();
}
