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
#include <qtxdg/xdgicon.h>
#include <qtxdg/xdgmime.h>

IconViewLabel::IconViewLabel(const QString & text, QGraphicsItem * parent)
    : QGraphicsTextItem(text, parent)
{
}



IconBase::IconBase(QGraphicsItem * parent)
    : QGraphicsItem(parent),
      m_highlight(false),
      m_launchMode(DesktopPlugin::SingleClick)
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
    QColor txt2 = bgText;

    bgText.setAlpha(m_highlight ? 80 : 40);

    painter->setBrush(QBrush(bgText, Qt::SolidPattern));

    painter->drawRoundedRect(textRect, 8, 8);


    textRect.adjust(4, 4, -4, -4);
    painter->drawText(textRect.translated(1, 1),
                      Qt::AlignCenter | Qt::TextWordWrap | Qt::TextWrapAnywhere | Qt::TextDontClip,
                      m_text);

    txt2.setAlpha(175);

    painter->setPen(QPen(txt2));
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
    if (m_launchMode == DesktopPlugin::DoubleClick)
        return;
//    qDebug() << "IconBase::mouseReleaseEvent" << event;
    switch (event->button())
    {
        case Qt::LeftButton:
            launchApp();
            break;
        default:
            // ignored
            break;
    }
}

void IconBase::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
    if (m_launchMode == DesktopPlugin::DoubleClick
        && event->button() == Qt::LeftButton)
    {
        launchApp();
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

    setText(xdg->localizedValue("Name").toString());
    setToolTip(xdg->localizedValue("Comment").toString());

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

    m_mimeInfo = new XdgMimeInfo(fi);
    setText(fi.fileName());
    setToolTip(file);
    if (fi.isDir())
    {
        QFileIconProvider ip;
        setIcon(ip.icon(fi));
    }
    else
        setIcon(m_mimeInfo->icon());
}

void FileIcon::launchApp()
{
    qDebug() << "FileIcon::launchApp()" << m_file << m_mimeInfo->mimeType();

    XdgDesktopFile* desktopFile = XdgDesktopFileCache::getDefaultApp(m_mimeInfo->mimeType());
    qDebug() << "FOUND:" << (desktopFile != 0);
    if (desktopFile)
        desktopFile->startDetached(m_file);
}

