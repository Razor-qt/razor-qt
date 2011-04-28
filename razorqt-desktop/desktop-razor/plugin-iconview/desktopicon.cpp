#include "desktopicon.h"
#include <QPainter>
#include <QPalette>
#include <QApplication>
#include <QIcon>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QDesktopServices>
#include <QUrl>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QtDebug>
#include <razorqt/xdgicon.h>


IconViewLabel::IconViewLabel(const QString & text, QGraphicsItem * parent)
    : QGraphicsTextItem(text, parent)
{
}



IconBase::IconBase(QGraphicsItem * parent)
    : QGraphicsItem(parent),
      m_highlight(false)
{
    // not yet setFlag(QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);
    setCursor(Qt::PointingHandCursor);
}

void IconBase::setIcon(const QIcon & icon)
{
    m_pm = icon.pixmap(32, 32, QIcon::Active);
    m_pm_hi = icon.pixmap(32, 32, QIcon::Selected);
}

void IconBase::setText(const QString & text)
{
    m_text = text;
}

QRectF IconBase::boundingRect() const
{
    return QRectF(0, 0, 80, 80);
}

void IconBase::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    QRect source(0, 0, 32, 32);
    int w = 80 / 2;
    int h = 80 / 2;
    int iw = 32 / 2;
    int ih = 32 / 2;
    QRect target(w - iw, h - ih - 10, 32, 32);
    painter->drawPixmap(target, m_highlight ? m_pm_hi : m_pm, source);

    QRectF textRect(0, 50, 80, 30);

    // text now - it has to follow potential QSS
    QColor txt = qApp->palette().color(QPalette::WindowText);
    painter->setPen(QPen(txt, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::BevelJoin));

    QColor bgText = qApp->palette().color(QPalette::Window);

    bgText.setAlpha(m_highlight ? 80 : 40);

    painter->setBrush(QBrush(bgText, Qt::SolidPattern));

    painter->drawRoundedRect(textRect, 8, 8);

    painter->drawText(textRect,
                      Qt::AlignCenter | Qt::TextWordWrap | Qt::TextWrapAnywhere | Qt::TextDontClip,
                      m_text);
}

void IconBase::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
//    qDebug() << "IconBase::contextMenuEvent" << event;
}

void IconBase::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
//    qDebug() << "IconBase::mousePressEvent" << event;
}

void IconBase::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
//    qDebug() << "IconBase::mouseReleaseEvent" << event;
    switch (event->button())
    {
        case Qt::LeftButton:
            launchApp();
            break;
        default:
            // ignored
            1;
    }
}

void IconBase::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
//    qDebug() << "IconBase::hoverEnterEvent" << event;
    m_highlight = true;
    
    // we can play with it as we want to...
    //QGraphicsDropShadowEffect * fx = new QGraphicsDropShadowEffect();
    //setGraphicsEffect(fx);

    QGraphicsItem::hoverEnterEvent(event);
}

void IconBase::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
//    qDebug() << "IconBase::hoverLeaveEvent" << event;
    m_highlight = false;
    //setGraphicsEffect(0);
    QGraphicsItem::hoverLeaveEvent(event);
}


DesktopIcon::DesktopIcon(XdgDesktopFile * xdg,
                         QGraphicsItem * parent
                        )
    : IconBase(parent)
{
    m_xdg = xdg;

    setText(xdg->value("Name").toString());
    setToolTip(xdg->value("Comment").toString());

    setIcon(xdg->icon(XdgIcon::defaultApplicationIcon()));
}

DesktopIcon::~DesktopIcon()
{
    delete m_xdg;
}

void DesktopIcon::launchApp()
{
    qDebug() << "DesktopIcon::launchApp()" << m_xdg->value("Exec");
    m_xdg->startDetached();
}

FileIcon::FileIcon(const QString & file,
                   QGraphicsItem * parent
                  )
    : IconBase(parent),
      m_file(file)
{
    QFileInfo fi(file);
    QFileIconProvider ip;

    setText(fi.fileName());
    setToolTip(file);
    setIcon(ip.icon(fi));
}

void FileIcon::launchApp()
{
    qDebug() << "FileIcon::launchApp()" << m_file;
    QDesktopServices::openUrl(QUrl(m_file));
}

