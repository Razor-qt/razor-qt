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

#include <QDesktopServices>
#include <QUrl>
#include <QFileIconProvider>
#include <QtDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QBitmap>

#include "razordeskicon.h"
#include <razorqt/xfitman.h>
#include <qtxdg/xdgicon.h>


RazorDeskIconBase::RazorDeskIconBase(
                             const QPoint & position,
                             QWidget * parent)
    : QAbstractButton(parent),
      m_mouseOver(false),
      m_display(0),
      m_displayHighlight(0)
{
    setAttribute(Qt::WA_AlwaysShowToolTips);

    qDebug() << "RazorDeskIconBase: initialising..." << parent;
    //setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    moveMe = false;
    movedMe = false;
    //QString name = QApplication::fontMetrics().elidedText(data->text(), Qt::ElideRight, 65);

    //TODO make this portable, read from config or anything else!
    QSize iconsize(32,32);
    setFixedSize(70,70);

    setIconSize(iconsize);
    
    if (! parent)
    {
        setAttribute(Qt::WA_X11NetWmWindowTypeDesktop);
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint | Qt::Dialog );
    }
    else
    {
        setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog );
    }

    setAttribute(Qt::WA_TranslucentBackground);
    setCursor(Qt::PointingHandCursor);

    connect(this, SIGNAL(clicked(bool)), this, SLOT(launchApp()));

    show();
    setPos(position);
}

RazorDeskIconBase::~RazorDeskIconBase()
{
    if (m_display)
        delete m_display;
    if (m_displayHighlight)
        delete m_displayHighlight;
    qDebug() << text() << " beeing shredded";
}

QSize RazorDeskIconBase::sizeHint() const
{
    return QSize(width(), height());
}

void RazorDeskIconBase::setPos(const QPoint & npos)
{
    qDebug() << "RazorDeskIconBase::setPos";
    // if we are in workspace-mode we can move the buttons using Qts move routine
    if (parent() != NULL)
        move(npos);
    else //else we need Xlib for moving, xlib is encapsulated by xfitman from librazorqt.
        xfitMan().moveWindow(effectiveWinId(), npos.x(), npos.y());
}

void RazorDeskIconBase::mouseMoveEvent(QMouseEvent* _event)
{
    if (moveMe)
    {
        if (firstGrab)
        {
            firstPos = _event->pos();
            firstGrab = false;
        }
        else
        {
            //qDebug() << "RazorDeskIconBase: MOVING TO:" << _event->globalPos();
            move(_event->globalPos()-firstPos);
            QAbstractButton::mouseMoveEvent(_event);
            movedMe = true;
        }
    }
}

void RazorDeskIconBase::mousePressEvent(QMouseEvent* _event)
{
    qDebug() << "RazorDeskIconBase: clicked!";

    movedMe = false;
    moveMe = true;
    firstGrab = true;
    QAbstractButton::mousePressEvent(_event);
}

void RazorDeskIconBase::mouseReleaseEvent(QMouseEvent* _event)
{
    qDebug() << "RazorDeskIconBase: mouserelease, checking for move!";
    moveMe = false;
    if (!movedMe)
    {
        qDebug() << "RazorDeskIconBase: not moved, so clicked!";
        setDown(false);
        if (m_launchMode == DesktopPlugin::SingleClick
            && _event->button() == Qt::LeftButton)
        {
            emit clicked();
        }
    }
    else
    {
        emit moved(pos());
        QAbstractButton::setDown(false);
    }
}

void RazorDeskIconBase::mouseDoubleClickEvent(QMouseEvent* _event)
{
    if (m_launchMode == DesktopPlugin::DoubleClick
        && _event->button() == Qt::LeftButton)
    {
        emit clicked();
    }
}


void RazorDeskIconBase::enterEvent(QEvent * event)
{
    m_mouseOver = true;
}

void RazorDeskIconBase::leaveEvent(QEvent * event)
{
    m_mouseOver = false;
}

void RazorDeskIconBase::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    if (m_mouseOver)
    {
        if (m_displayHighlight)
            painter.drawPixmap(0, 0, *m_displayHighlight);
    }
    else
    {
        if (m_display)
            painter.drawPixmap(0, 0, *m_display);
    }
}

void RazorDeskIconBase::setIcon(const QIcon & icon)
{
    qDebug() << "RazorDeskIconBase::setIcon";
    QAbstractButton::setIcon(icon);

    m_display = initialPainting(QIcon::Normal);
    Q_ASSERT(m_display);
    m_displayHighlight = initialPainting(QIcon::Selected);
    Q_ASSERT(m_displayHighlight);
}

QPixmap * RazorDeskIconBase::initialPainting(QIcon::Mode mode)
{
    qDebug() << "RazorDeskIconBase::initialPainting";
    
    if (icon().isNull())
    {
        qDebug() << "RazorDeskIconBase::setPos - icon() is null. Skipping for now.";
        return 0;
    }
    
    QPixmap * pm = new QPixmap(70, 70);
    pm->fill(QColor(0,0,0,0));

    QPainter painter(pm);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    painter.setRenderHint(QPainter::NonCosmeticDefaultPen);
    
    // now the icon
    QPixmap appIcon = icon().pixmap(iconSize(), mode);
    // HACK: in some cases we can get larger icon than expected so rescale
    //       it with brute force if it's required...
    if (appIcon.size().width() > iconSize().width())
        appIcon = appIcon.scaled(iconSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QRect source(0, 0, 32, 32);
    int w = pm->width() / 2;
    int h = pm->height() / 2;
    int iw = iconSize().width() / 2;
    int ih = iconSize().height() / 2;
    QRect target(w - iw, h - ih - 10,
                 iconSize().width(), iconSize().height());
    painter.drawPixmap(target, appIcon, source);

    // text now - it has to follow potential QSS
    QColor txt = palette().color(QPalette::WindowText);
    painter.setPen(txt);
    painter.setBrush(palette().color(QPalette::Window));
    painter.drawText(QRectF(2, h+ih-10, pm->width()-4, pm->height()-h-ih+10),
                     Qt::AlignCenter | Qt::TextWordWrap | Qt::TextIncludeTrailingSpaces | Qt::TextDontClip,
                     text());
    painter.end();

    pm->setMask(pm->createHeuristicMask());
    setMask(pm->mask());
    
    return pm;
}


RazorDeskIconDesktop::RazorDeskIconDesktop(XdgDesktopFile * xdg,
                                           const QPoint & position,
                                           QWidget * parent
                                          )
    : RazorDeskIconBase(position, parent)
{
    m_xdg = xdg;

    setText(xdg->value("Name").toString());
    setToolTip(xdg->value("Comment").toString());

    setIcon(xdg->icon(XdgIcon::defaultApplicationIcon()));
}

RazorDeskIconDesktop::~RazorDeskIconDesktop()
{
    delete m_xdg;
}

void RazorDeskIconDesktop::launchApp()
{
    qDebug() << "RazorDeskIconDesktop::launchApp()" << m_xdg->value("Exec");
    m_xdg->startDetached();
}

RazorDeskIconFile::RazorDeskIconFile(const QString & file,
                                     const QPoint & position,
                                     QWidget * parent
                                    )
    : RazorDeskIconBase(position, parent),
      m_file(file)
{
    QFileInfo fi(file);
    QFileIconProvider ip;

    setText(fi.fileName());
    setToolTip(file);
    setIcon(ip.icon(fi));
}

void RazorDeskIconFile::launchApp()
{
    qDebug() << "RazorDeskIconFile::launchApp()" << m_file;
    QDesktopServices::openUrl(QUrl(m_file));
}
