/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
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

#include <QtGui/QDesktopWidget>
#include <razorqt/razorsettings.h>
#include "notificationarea.h"
#include <QtDebug>

NotificationArea::NotificationArea(QWidget *parent)
    : QScrollArea(parent),
      m_settings(new RazorSettings("notifications", this))
{
    setObjectName("NotificationArea");
    qDebug() << "AREA" << m_settings->fileName() << m_settings;

    setWindowFlags(Qt::X11BypassWindowManagerHint
                   | Qt::FramelessWindowHint
                   | Qt::WindowStaysOnTopHint);
    // Hack to ensure the fully transparent QGraphicsView background
    QPalette palette;
    palette.setBrush(QPalette::Base, Qt::NoBrush);
    setPalette(palette);
    // Required to display wallpaper
    setAttribute(Qt::WA_TranslucentBackground);
    // no border at all finally
    setFrameShape(QFrame::NoFrame);

    m_layout = new NotificationLayout(this);
    setWidget(m_layout);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // TODO/FXIME: take care about top-left positioning
    QDesktopWidget dw;
    QRect position = dw.availableGeometry(this);
    move(position.x(), position.y());

    connect(m_layout, SIGNAL(allNotificationsClosed()), this, SLOT(close()));
    connect(m_layout, SIGNAL(notificationAvailable()), this, SLOT(show()));
    connect(m_layout, SIGNAL(heightChanged(int)), this, SLOT(setHeight(int)));

    // TODO/FIXME: WTF?! Why it does not synchronize?
    connect(m_settings, SIGNAL(settingsChanged()),
            this, SLOT(applySettings()));
    applySettings();
}

void NotificationArea::setHeight(int contentHeight)
{
    // TODO/FXIME: take care about top-left positioning
    QDesktopWidget dw;
    int h = dw.availableGeometry(this).height();
    int w = dw.availableGeometry(this).width();
    int safeHeight = contentHeight > h ? h : contentHeight;
    int x, y;

    if (m_placement == "bottom-right")
    {
        x = w - width() - m_spacing;
        y = h - safeHeight;
    }
    else if (m_placement == "bottom-left")
    {
        x = dw.availableGeometry(this).x() + m_spacing;
        y = h - safeHeight;
    }
    else if (m_placement == "top-right")
    {
        x = w - width() - m_spacing;
        y = dw.availableGeometry(this).y();
    }
    else if (m_placement == "top-left")
    {
        
        x = dw.availableGeometry(this).x() - m_spacing;
        y = dw.availableGeometry(this).y() - m_spacing;
    }
    else
    {
        x = w - width() - m_spacing;
        y = h - safeHeight;
    }

    move(x, y);
    resize(width(), safeHeight);
    // always show the latest notification
    ensureVisible(0, contentHeight, 0, 0);
}

void NotificationArea::applySettings()
{
    qDebug() << "applySettings";
    m_placement = m_settings->value("placement", "bottom-right").toString().toLower();
    // width settings has to go before layout->setSpacing()
    int w = m_settings->value("width", 300).toInt();
    setMaximumWidth(w);
    setMinimumWidth(w);

    m_spacing = m_settings->value("spacing", 6).toInt();
    m_layout->setSizes(m_spacing, w);
        
    this->setHeight(widget()->height());
}
