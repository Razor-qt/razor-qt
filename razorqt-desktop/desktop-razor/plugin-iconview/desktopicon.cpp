#include "desktopicon.h"
#include <QPainter>
#include <QPalette>
#include <QApplication>


DesktopIcon::DesktopIcon(const QPixmap & pixmap, const QString & text, const QPointF & position)
    : m_pm(pixmap),
      m_text(text)
{
    setPos(position);
    setFlag(QGraphicsItem::ItemIsMovable);
}

QRectF DesktopIcon::boundingRect() const
{
    return QRectF(0, 0, 80, 80);
}

void DesktopIcon::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    QRect source(0, 0, 32, 32);
    int w = 80 / 2;
    int h = 80 / 2;
    int iw = 32 / 2;
    int ih = 32 / 2;
    QRect target(w - iw, h - ih - 10, 32, 32);
    painter->drawPixmap(target, m_pm, source);

    QRectF textRect(0, 50, 80, 30);

    // text now - it has to follow potential QSS
    QColor txt = qApp->palette().color(QPalette::WindowText);
    painter->setPen(QPen(txt, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::BevelJoin));

    QColor bgText = qApp->palette().color(QPalette::Window);
    bgText.setAlpha(80);

    painter->setBrush(QBrush(bgText, Qt::SolidPattern));

    painter->drawRoundedRect(textRect, 8, 8);

    painter->drawText(textRect,
                      Qt::AlignCenter | Qt::TextWordWrap | Qt::TextIncludeTrailingSpaces | Qt::TextDontClip,
                      m_text);

}
