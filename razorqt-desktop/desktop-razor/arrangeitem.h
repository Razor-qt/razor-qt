#ifndef ARRANGEITEM_H
#define ARRANGEITEM_H

#include <QGraphicsObject>
#include "desktopwidgetplugin.h"


class ArrangeItem : public QGraphicsObject
{
    Q_OBJECT

public:
    ArrangeItem(DesktopWidgetPlugin * plugin, const QString & text, const QRectF & rect, bool editable = true, QGraphicsItem * parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

    DesktopWidgetPlugin * plugin();
    bool editable() { return m_editable; }

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

private:
    enum ChangeMode {
        C_TL, C_T, C_TR,
        C_L,  C_C, C_R,
        C_BL, C_B, C_BR
    };

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
