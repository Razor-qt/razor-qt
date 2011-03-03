#include <QDesktopServices>
#include <QUrl>
#include <QFileIconProvider>

#include "razordeskicon.h"
#include "razor.h"
#include <razorqt/xfitman.h>

/**
 * @file RazorDeskIconBase.cpp
 * @author Christopher "VdoP" Regali
 * @brief implements the class RazorDeskIconBase
 */


RazorDeskIconBase::RazorDeskIconBase(
                             const QPoint & position,
                             QWidget * parent)
    : QAbstractButton(parent),
      m_mouseOver(false),
      m_display(0)
{
    if (!parent)
        setParent(QApplication::desktop());

    setAttribute(Qt::WA_AlwaysShowToolTips);

    qDebug() << "RazorDeskIconBase: initialising..." << parent;
    //setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    moveMe = false;
    movedMe = false;
    //QString name = QApplication::fontMetrics().elidedText(data->text(), Qt::ElideRight, 65);

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
    // if we are in workspace-mode we can move the buttons using Qts move routine
    if (parent() != NULL)
        move(npos);
    else //else we need Xlib for moving, xlib is encapsulated by xfitman from librazorqt.
        Razor::getInstance().getxfitman()->moveWindow(effectiveWinId(), npos.x(), npos.y());
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
        if (_event->button() == Qt::LeftButton)
            emit clicked();
    }
    else
    {
        emit moved(pos());
        QAbstractButton::setDown(false);
    }
}

void RazorDeskIconBase::enterEvent(QEvent * event)
{
    //qDebug() << "enterEvent";
    m_mouseOver = true;
    clearMask();
    setMask(m_displayHighlight->mask());
    update();
}

void RazorDeskIconBase::leaveEvent(QEvent * event)
{
    //qDebug() << "leaveEvent";
    m_mouseOver = false;
    clearMask();
    setMask(m_display->mask());
    update();
}

void RazorDeskIconBase::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if (! m_display)
        initialPainting();

    if (m_mouseOver)
    {
        painter.drawPixmap(0, 0, *m_displayHighlight);
    }
    else
    {
        painter.drawPixmap(0, 0, *m_display);
    }
}

void RazorDeskIconBase::initialPainting()
{
    Q_ASSERT(!m_display);
    
    m_display = new QPixmap(70, 70);
    m_display->fill(QColor(0,0,0,0));

    QPainter painter(m_display);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    painter.setRenderHint(QPainter::NonCosmeticDefaultPen);
    
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
    painter.setBrush(palette().color(QPalette::Window));
//    painter.drawRoundedRect(QRectF(2, h+ih-10, m_display->width()-4, m_display->height()-h-ih+10),
//                            6, 6);
    painter.drawText(QRectF(2, h+ih-10, m_display->width()-4, m_display->height()-h-ih+10),
                     Qt::AlignCenter | Qt::TextWordWrap | Qt::TextIncludeTrailingSpaces,
                     text());
    painter.end();
                    
    QBitmap mask = m_display->createHeuristicMask();
    m_display->setMask(mask);

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
    setIcon(Razor::getInstance().geticontheme()->getIconNG(xdg->value("Icon").toString()));
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
