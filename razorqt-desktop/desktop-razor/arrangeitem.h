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

#ifndef ARRANGEITEM_H
#define ARRANGEITEM_H

#include <QtGui/QGraphicsObject>
#include "desktopwidgetplugin.h"

class ArrangeItem : public QGraphicsObject
{
    Q_OBJECT

public:
    ArrangeItem(QGraphicsItem * related, DesktopWidgetPlugin * plugin, const QString & text, const QRectF & rect, bool editable = true, QGraphicsItem * parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

    DesktopWidgetPlugin * plugin();
    QGraphicsItem * related() { return m_related; }
    bool editable() { return m_editable; }

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);

private:
    enum ChangeMode {
        C_TL, C_T, C_TR,
        C_L,  C_C, C_R,
        C_BL, C_B, C_BR
    };

    QGraphicsItem * m_related;
    DesktopWidgetPlugin * m_plugin;
    ChangeMode m_mode;
    QRectF m_rect;
    QRectF m_prevRect;
    QPointF m_prevPos;
    QString m_text;
    bool m_editable;
    bool m_highlight;

    QTimer * m_timer;

    QCursor getCursorByPos(const QPointF & position);

private slots:
    void setCursorByTimer();
};

#endif
