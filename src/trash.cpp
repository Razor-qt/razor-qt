////////////////////////////////////////
//  File      : trash.cpp             //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "trash.h"

////////////////////////////////////////

Trash::Trash(QWidget *parent) : QWidget(parent)
{
    read_settings();
    init();
    show();
}

Trash::~Trash()
{
    delete antico;
}

void Trash::init()
{
    setFixedSize(100, 50);
    pix = QPixmap(trash_pix); // set default pixmap
    zoom = false;
    trash_dlg = new Trashdialog(this); // create the Trashdialog
}

void Trash::read_settings()
{
    // get style path
    antico = new QSettings(QDir::homePath() + "/.razor/razor.cfg", QSettings::IniFormat, this);
    antico->beginGroup("Style");
    stl_name = antico->value("name").toString();
    stl_path = antico->value("path").toString();
    antico->endGroup(); //Style
    // get style values
    QSettings *style = new QSettings(stl_path + stl_name, QSettings::IniFormat, this);
    style->beginGroup("Trash");
    trash_pix = stl_path + style->value("trash_pix").toString();
    trash_col = style->value("name_color").value<QColor>();
    style->endGroup(); //Trash
}

void Trash::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setWindow(-50, -50, 100, 50);
    painter.setOpacity(0.5);
    painter.setPen(Qt::black);
    painter.drawText(-49, -14, 100, 20, Qt::AlignHCenter, tr("Trash")); // shadow Trash name
    painter.setOpacity(1);
    painter.setPen(trash_col);
    painter.drawText(-50, -15, 100, 20, Qt::AlignHCenter, tr("Trash")); // Trash name
    
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    if (zoom)
    {
        painter.drawPixmap(QRect(-18, -50, 36, 36), pix, QRect(0, 0, pix.width(), pix.height()));// Trash pix
    }
    else
    {
        painter.drawPixmap(QRect(-16, -50, 32, 32), pix, QRect(0, 0, pix.width(), pix.height()));// Trash pix
    }
}

void Trash::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mousepos = event->pos();
        grabMouse(QCursor(Qt::SizeAllCursor));
    }
}

void Trash::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p = event->globalPos()-mousepos;
    move(p.x(), p.y());
}

void Trash::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    releaseMouse();
    // save new Trash position on desk writing on antico.cfg
    antico->beginGroup("Trash");
    antico->setValue("pos", pos());
    antico->endGroup(); // Trash
}

void Trash::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        trash_dlg->update();
        trash_dlg->exec();
    }
}

void Trash::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    zoom = true;
    update();
    
    int counter = trash_dlg->files_counter(); // get the trash files number
 
    if (counter > 0)
        setToolTip(QString(tr("The trash contains ")).append(QString("%1").arg(counter)).append(tr(" entries")));
    else
        setToolTip(tr("The trash is empty"));
}

void Trash::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    zoom = false;
    update();
}

void Trash::update_style()
{
    read_settings();
    init();
    update();
}
