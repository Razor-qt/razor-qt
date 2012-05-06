/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2012 A-MeGo
 * http://a-mego.ru
 * Authors:
 *   Alexey Nosov <nay@a-mego.ru>
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

#include <QtDebug>
#include <QPalette>
#include <QPropertyAnimation>
#include "notepad.h"

EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_CPP(Notepad)


Notepad::Notepad(QGraphicsScene *scene, const QString &configId, RazorSettings *config)
    : DesktopWidgetPlugin(scene, configId, config)
{
    setObjectName("Notepad");
    m_config->beginGroup(configId);
	QString text = m_config->value("text", "").toString();
	int x = m_config->value("x", 0).toInt();
	int y = m_config->value("y", 0).toInt();
    int w = m_config->value("w", 0).toInt();
    int h = m_config->value("h", 0).toInt();
	int pos = m_config->value("pos", 0).toInt();
    m_config->endGroup();

	resize(w, h);
	move(x, y);
	QPalette palette;
	palette.setColor(backgroundRole(), Qt::transparent);
	setPalette(palette);
   
	layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	setLayout(layout);
	
	win = new NotepadWin(this, &Notepad::save);
	layout->addWidget(win);
	win->setTextAndPos(text, pos);
}

Notepad::~Notepad()
{
    delete win;
	delete layout;
}

    
QString Notepad::info()
{
    return QObject::tr("Display a notepad");
}

QString Notepad::instanceInfo()
{
    return QObject::tr("Notepad:") + " " + m_configId;
}

void Notepad::setSizeAndPosition(const QPointF &position, const QSizeF &size)
{
    qDebug() << "Moving to" << position << "resizing" << size;
    move(position.x(), position.y());
    resize(size.width(), size.height());
    win->setParentSize(size);
    // "cool" display FX - the main part
    QPropertyAnimation * animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(500);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start();
}

void Notepad::save()
{
    m_config->beginGroup(m_configId);
    m_config->setValue("plugin", "notepad");
    m_config->setValue("x", pos().x());
    m_config->setValue("y", pos().y());
    m_config->setValue("w", size().width());
    m_config->setValue("h", size().height());
    m_config->setValue("text", win->text());
	m_config->setValue("pos", win->pos());
    m_config->endGroup();
}

void Notepad::configure()
{
    save();
}
