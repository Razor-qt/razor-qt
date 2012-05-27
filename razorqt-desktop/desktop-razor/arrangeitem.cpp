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

#include <QtGui/QPainter>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QCursor>
#include <QtCore/QTimer>
#include <QtGui/QGraphicsScene>
#include <QtCore/QtDebug>

#include "arrangeitem.h"

ArrangeItem::ArrangeItem(QGraphicsItem * related, DesktopWidgetPlugin * plugin, const QString & text, const QRectF & rect, bool editable, QGraphicsItem * parent)
    : QGraphicsObject(parent),
      m_related(related),
      m_plugin(plugin),
      m_mode(C_C),
      m_rect(rect),
      m_text(text),
      m_editable(editable)
{
    setPos(rect.x(), rect.y());

    m_timer = new QTimer(this);

    if (m_editable)
    {
        m_timer->setInterval(500);
        connect(m_timer, SIGNAL(timeout()), this, SLOT(setCursorByTimer()));

        setAcceptHoverEvents(true);
        setAcceptedMouseButtons(Qt::LeftButton);
        setFlag(QGraphicsItem::ItemIsMovable);
        setZValue(2);
    }
    else
        setZValue(1);
}

QRectF ArrangeItem::boundingRect() const
{
    return QRectF(0, 0, m_rect.width(), m_rect.height());
}

void ArrangeItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    QFont f = painter->font();
    f.setPointSize(m_editable ? f.pointSize()*2 : f.pointSize()*6);
    painter->setFont(f);

    painter->setPen(QPen(Qt::white, 1));
    if (m_editable)
        painter->setBrush(QColor(0, 0, 0, m_highlight ? 200 : 50));
    else
        painter->setBrush(QColor(0, 0, 0, 50));

    painter->drawRect(1, 1, boundingRect().width()-1, boundingRect().height()-1);
    
    if (!m_editable)
    {
        painter->setPen(Qt::cyan);
    }
    
    painter->drawText(boundingRect(),
                      Qt::AlignCenter | Qt::TextWordWrap | Qt::TextWrapAnywhere | Qt::TextDontClip,
                      m_text);
}

DesktopWidgetPlugin * ArrangeItem::plugin()
{
    //qDebug() << "ArrangeItem::plugin" << m_text;
    Q_ASSERT(m_plugin);
    return m_plugin;
}


void ArrangeItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    m_highlight = true;
    m_timer->start();
    QGraphicsItem::hoverEnterEvent(event);
}

void ArrangeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    m_highlight = false;
    m_timer->stop();
    setCursor(Qt::ArrowCursor);
    QGraphicsItem::hoverLeaveEvent(event);
}

void ArrangeItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mousePressEvent(event);
    setCursor(getCursorByPos(event->pos()));
    m_timer->stop();
}

void ArrangeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    m_timer->start();
}

void ArrangeItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    m_prevPos = pos();
    m_prevRect = m_rect;

    qreal x = event->scenePos().x();
    qreal y = event->scenePos().y();
    QPointF position = pos();
    qreal diffx = (x - position.x()) / 2.0;
    qreal diffy = (y - position.y()) / 2.0;
    qreal diffxw = (x - (position.x() + m_rect.width()));
    qreal diffyw = (y - (position.y() + m_rect.height()));
    
    prepareGeometryChange();

    switch (m_mode)
    {
        case C_TL:
            setPos(position.x() + diffx, position.y() + diffy);
            m_rect.setHeight(m_rect.height() - diffy);
            m_rect.setWidth(m_rect.width() - diffx);
            break;
        case C_TR:
            setPos(position.x(), position.y() + diffy);
            m_rect.setHeight(m_rect.height() - diffy);
            m_rect.setWidth(m_rect.width() + diffxw/2.0);
            break;
        case C_BL:
            setPos(position.x() + diffxw/2.0, position.y());
            m_rect.setHeight(m_rect.height() + diffyw/2.0);
            m_rect.setWidth(m_rect.width() - diffxw/2.0);
            break;
        case C_BR:
            m_rect.setHeight(m_rect.height() + diffyw/2.0);
            m_rect.setWidth(m_rect.width() + diffxw/2.0);
            break;
        case C_T:
            setPos(position.x(), position.y() + diffy);
            m_rect.setHeight(m_rect.height() - diffy);
            break;
        case C_B:
            m_rect.setHeight(m_rect.height() + diffyw);
            break;
        case C_L:
            setPos(position.x()+diffx, position.y());
            m_rect.setWidth(m_rect.width() - diffx);
            break;
        case C_R:
            m_rect.setWidth(m_rect.width() + diffxw);
            break;
        case C_C:
        default:
            QGraphicsItem::mouseMoveEvent(event);
    }

    // scene boundaries
    QRectF sc(scene()->sceneRect());
    QRectF it(sceneBoundingRect());
    if (it.x() < sc.x() || it.y() < sc.y()
        || it.width()+it.x() > sc.width() || it.height()+it.y() > sc.height())
    {
        setPos(m_prevPos);
        m_rect = m_prevRect;
        return;
    }
    
    // colliding plugins/items
    QList<QGraphicsItem*> colliding = collidingItems();
    foreach (QGraphicsItem* i, colliding)
    {
        ArrangeItem * item = dynamic_cast<ArrangeItem*>(i);
        if (item && item->editable())
        {
            setPos(m_prevPos);
            m_rect = m_prevRect;
            return;
        }
    }
}

void ArrangeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
    if (m_editable)
        m_plugin->configure();
}

QCursor ArrangeItem::getCursorByPos(const QPointF & position)
{
    if (! m_editable)
        return Qt::ArrowCursor;

    qreal x = position.x();
    qreal y = position.y();
    int size = 10;
    QRectF itemSize = boundingRect();
    
    //qDebug() << "getCursorByPos" << position << x << y << itemSize << itemSize.width() << itemSize.height();
    // resizing
    if (x < size && y < size)
    {
        // top-left
        m_mode = C_TL;
        return Qt::SizeFDiagCursor;
    }
    else if (x > itemSize.width()-size && y < size)
    {
        // top-right
        m_mode = C_TR;
        return Qt::SizeBDiagCursor;
    }
    else if (x < size && y > itemSize.height()-size)
    {
        // bottom-left
        m_mode = C_BL;
        return Qt::SizeBDiagCursor;
    }
    else if (x > itemSize.width()-size && y > itemSize.height()-size)
    {
        // bottom-right
        m_mode = C_BR;
        return Qt::SizeFDiagCursor;
    }
    else if (x < size)
    {
        // left
        m_mode = C_L;
        return Qt::SizeHorCursor;
    }
    else if (x > itemSize.width()-size)
    {
        // right
        m_mode = C_R;
        return Qt::SizeHorCursor;
    }
    else if (y < size)
    {
        // top
        m_mode = C_T;
        return Qt::SizeVerCursor;
    }
    else if (y > itemSize.height()-size)
    {
        // top or bottom
        m_mode = C_B;
        return Qt::SizeVerCursor;
    }

    // center == moving
    m_mode = C_C;
    return Qt::SizeAllCursor;
}

void ArrangeItem::setCursorByTimer()
{
    setCursor(getCursorByPos(mapFromScene(QCursor::pos())));
    m_timer->start();
}
