#include "razordeskicon.h"
#include "razor.h"
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
      m_exec(exec)
{
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

    connect(this, SIGNAL(clicked(bool)), this, SLOT(launchApp()));

    show();
}

RazorDeskIcon::~RazorDeskIcon()
{
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
    Q_UNUSED(_event);
    qDebug() << "Razordeskicon: mouserelease, checking for move!";
    moveMe = false;
    if (!movedMe)
    {
        qDebug() << "Razordeskicon: not moved, so clicked!";
        setDown(false);
        emit clicked();
    }
    else
    {
        emit moved(pos());
        QAbstractButton::setDown(false);
    }
}

void RazorDeskIcon::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0,0,width(),height()), Qt::transparent);

    // now the icon
    QPixmap pm = icon().pixmap(iconSize(), isDown() ? QIcon::Selected : QIcon::Selected);
    QRect source(0, 0, 32, 32);
    int w = width() / 2;
    int h = height() / 2;
    int iw = iconSize().width() / 2;
    int ih = iconSize().height() / 2;
    QRect target(w - iw, h - ih - 10,
                 iconSize().width(), iconSize().height());
    //qDebug() << target << w << h << iw << ih;
    painter.drawPixmap(target, pm, source);
    // text now
    painter.setPen(Qt::black);
    painter.drawText(QRectF(2, h+ih-10, width()-4, height()-h-ih+10),
                     Qt::AlignCenter | Qt::TextWordWrap | Qt::TextIncludeTrailingSpaces,
                     text());
}

void RazorDeskIcon::launchApp()
{
    qDebug() << "RazorDeskIcon::launchApp()" << m_exec;
    QProcess::startDetached(m_exec);
}
