#include "razordeskicon.h"
#include "razor.h"
#include <razorqt/xfitman.h>

/**
 * @file razordeskicon.cpp
 * @author Christopher "VdoP" Regali
 * @brief implements the class Razordeskicon
 */


RazorDeskIcon::RazorDeskIcon(const QString & exec,
                             const QIcon & icon,
                             const QString & text,
                             const QString & comment,
                             const QPoint & position,
                             QWidget * parent)
    : QAbstractButton(parent),
      m_exec(exec),
      m_mouseOver(false),
      m_display(0)
{
    if (!parent)
        setParent(QApplication::desktop());

    setAttribute(Qt::WA_AlwaysShowToolTips);

    qDebug() << "Razordeskicon: initialising..." << parent;
    //setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    moveMe = false;
    movedMe = false;
    //QString name = QApplication::fontMetrics().elidedText(data->text(), Qt::ElideRight, 65);
    setText(text);

    setToolTip(comment);
    setIcon(icon);

    //TODO make this portable, red from config or anything else!
    QSize iconsize(32,32);
    setFixedSize(70,70);

    setIconSize(iconsize);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint | Qt::Dialog );
    setAttribute(Qt::WA_X11NetWmWindowTypeDesktop);
    setAttribute(Qt::WA_TranslucentBackground);

    connect(this, SIGNAL(clicked(bool)), this, SLOT(launchApp()));

    show();
    setPos(position);
}

RazorDeskIcon::~RazorDeskIcon()
{
    if (m_display)
        delete m_display;
    if (m_displayHighlight)
        delete m_displayHighlight;
    qDebug() << text() << " beeing shredded";
}

QSize RazorDeskIcon::sizeHint() const
{
    return QSize(width(), height());
}

void RazorDeskIcon::setPos(const QPoint & npos)
{
    // if we are in workspace-mode we can move the buttons using Qts move routine
    if (parent() != NULL)
        move(npos);
    else //else we need Xlib for moving, xlib is encapsulated by xfitman from librazorqt.
        Razor::getInstance().getxfitman()->moveWindow(effectiveWinId(), npos.x(), npos.y());
}

void RazorDeskIcon::mouseMoveEvent(QMouseEvent* _event)
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
            //qDebug() << "Razordeskicon: MOVING TO:" << _event->globalPos();
            move(_event->globalPos()-firstPos);
            QAbstractButton::mouseMoveEvent(_event);
            movedMe = true;
        }
    }
}

void RazorDeskIcon::mousePressEvent(QMouseEvent* _event)
{
    qDebug() << "Razordeskicon: clicked!";

    movedMe = false;
    moveMe = true;
    firstGrab = true;
    QAbstractButton::mousePressEvent(_event);
}

void RazorDeskIcon::mouseReleaseEvent(QMouseEvent* _event)
{
    qDebug() << "Razordeskicon: mouserelease, checking for move!";
    moveMe = false;
    if (!movedMe)
    {
        qDebug() << "Razordeskicon: not moved, so clicked!";
        setDown(false);
        if (_event->button() == Qt::LeftButton)
            emit clicked();
    }
    else
    {
        emit moved(pos());
        QAbstractButton::setDown(false);
    }
}

void RazorDeskIcon::enterEvent(QEvent * event)
{
    m_mouseOver = true;
    update();
}

void RazorDeskIcon::leaveEvent(QEvent * event)
{
    m_mouseOver = false;
    update();
}

void RazorDeskIcon::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if (! m_display)
        initialPainting();
    
    if (m_mouseOver)
    {
        clearMask();
        setMask(m_displayHighlight->mask());
        painter.drawPixmap(0, 0, *m_displayHighlight);
    }
    else
    {
        clearMask();
        setMask(m_display->mask());
        painter.drawPixmap(0, 0, *m_display);
    }
}

void RazorDeskIcon::initialPainting()
{
    qDebug() << "initialPainting";
    Q_ASSERT(!m_display);
    
    m_display = new QPixmap(70, 70);
    m_display->fill(QColor(0,0,0,0));

    QPainter painter(m_display);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // now the icon
    QPixmap pm = icon().pixmap(iconSize(), isDown() ? QIcon::Selected : QIcon::Selected);
    QRect source(0, 0, 32, 32);
    int w = m_display->width() / 2;
    int h = m_display->height() / 2;
    int iw = iconSize().width() / 2;
    int ih = iconSize().height() / 2;
    QRect target(w - iw, h - ih - 10,
                 iconSize().width(), iconSize().height());
    //qDebug() << target << w << h << iw << ih;
    painter.drawPixmap(target, pm, source);

    // text now - it has to follow potential QSS
    QColor txt = palette().color(QPalette::WindowText);
    painter.setPen(txt);
    painter.drawText(QRectF(2, h+ih-10, m_display->width()-4, m_display->height()-h-ih+10),
                     Qt::AlignCenter | Qt::TextWordWrap | Qt::TextIncludeTrailingSpaces,
                     text());
    
    //txt.setAlpha(150);
    //painter.drawRoundedRect(m_display->rect(), 6, 6);
    painter.end();
                    
    QBitmap mask = m_display->createHeuristicMask();
    //QBitmap mask(*m_display);
    
    m_display->setMask(mask);
//    m_display->save("bar.png");
//    mask.save("foo.png");

    m_displayHighlight = new QPixmap(70, 70);
    m_displayHighlight->fill(QColor(0,0,0,100));
    QPainter hpainter(m_displayHighlight);
    QColor bgcolor = palette().color(QPalette::Window);
    QColor hicolor = palette().color(QPalette::WindowText);
    hpainter.setPen(QPen(hicolor, 1));
    hpainter.setBrush(bgcolor);
    hpainter.drawRoundedRect(1, 1, 67, 67, 9, 9);
    hpainter.drawPixmap(m_displayHighlight->rect(), m_display->copy(), m_display->rect());
    hpainter.end();
    //m_displayHighlight->setMask(m_displayHighlight->createHeuristicMask());
    
    m_displayHighlight->save("foo.png");
}

void RazorDeskIcon::launchApp()
{
    qDebug() << "RazorDeskIcon::launchApp()" << m_exec;
    QProcess::startDetached(m_exec);
}
