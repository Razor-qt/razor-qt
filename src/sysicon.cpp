////////////////////////////////////////
//  File      : sysicon.cpp           //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "sysicon.h"

////////////////////////////////////////

Sysicon::Sysicon(Frame *frame, QWidget *parent) : QWidget(parent)
{
    frm = frame;
    read_settings();
    setContentsMargins(0, 0, 0, 0);
    setFixedSize(32, 32);
    setToolTip(frm->cl_name());
    bdr_width = 1;
}

Sysicon::~Sysicon()
{
    delete frm;
}

void Sysicon::read_settings()
{
    // get style path
    QSettings *antico = new QSettings(QDir::homePath() + "/.razor/razor.cfg", QSettings::IniFormat, this);
    antico->beginGroup("Style");
    QString stl_name = antico->value("name").toString();
    QString stl_path = antico->value("path").toString();
    antico->endGroup(); //Style
    // get style values
    QSettings *style = new QSettings(stl_path +stl_name, QSettings::IniFormat, this);
    style->beginGroup("Sysicon");
    s_icon_pix = stl_path + style->value("s_icon_pix").toString();
    pix = QPixmap(s_icon_pix);
    style->endGroup(); //Sysicon
    style->beginGroup("Other");
    close_dock_pix = stl_path + style->value("close_dock_pix").toString();
    style->endGroup(); //Other
    style->beginGroup("Dockicon");
    title_color = style->value("title_color").value<QColor>();
    style->endGroup(); //Dockicon
}

void Sysicon::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(title_color, bdr_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawRoundedRect(0, 0, width(), height(), 5, 5);
    painter.drawPixmap(QRect(0, 0, width(), height()), pix, QRect(0, 0, pix.width(), pix.height()));// sysicon pixmap
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawPixmap(QRect(3, 3, height()-6, height()-6), frm->cl_icon(), QRect(0, 0, frm->cl_icon().width(), frm->cl_icon().height()));// sysicon
}

void Sysicon::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit show_sys(this);
        frm->raise_it();
        close();
    }
    if (event->button() == Qt::RightButton)
    {
        QMenu *menu = new QMenu(this);
        QAction *close = menu->addAction(QIcon(close_dock_pix), "Close");
        menu->popup(event->globalPos());
        connect(close, SIGNAL(triggered()), this, SLOT(sys_close()));
    }
}

void Sysicon::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    bdr_width = 2;
    update();
}

void Sysicon::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    bdr_width = 1;
    update();
}

void Sysicon::sys_close()
{
    emit destroy_sys(this);
    frm->destroy_it();
    close();
}

void Sysicon::update_style()
{
    read_settings();
    update();
}

