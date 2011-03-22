#ifndef DESKTOPICON_H
#define DESKTOPICON_H

#include <QGraphicsItem>

class DesktopIcon : public QGraphicsItem
{
public:
    DesktopIcon(const QPixmap & pixmap, const QString & text, const QPointF & position);

    QRectF boundingRect() const;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

private:
    QPixmap m_pm;
    QString m_text;
};

#endif // DESKTOPICON_H
