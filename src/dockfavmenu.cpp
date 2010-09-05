#include "dockfavmenu.h"


Dockfavmenu::Dockfavmenu(QMenu *menu, QWidget *parent) : QWidget(parent)
{
    cat_menu = menu;
    bdr_width = 1;
    zoom = false;
    setToolTip(cat_menu->title());
    read_settings();
}

Dockfavmenu::~Dockfavmenu()
{
    delete cat_menu;
}

void Dockfavmenu::read_settings()
{
    // get style path
    QSettings *antico = new QSettings(QDir::homePath() + "/.razor/razor.cfg", QSettings::IniFormat, this);
    antico->beginGroup("Style");
    QString stl_name = antico->value("name").toString();
    QString stl_path = antico->value("path").toString();
    antico->endGroup(); //Style
    // get style values
    QSettings *style = new QSettings(stl_path + stl_name, QSettings::IniFormat, this);
    style->beginGroup("Favmenu");
    d_menu_pix = stl_path + style->value("d_icon_pix").toString();
    title_color = style->value("title_color").value<QColor>();
    style->endGroup(); //Dockicon
    style->beginGroup("Dockbar");
    dock_position = style->value("dock_position").toInt();
    style->endGroup(); //Dockbar
    back_pix = QPixmap(d_menu_pix);
    cat_pix = cat_menu->icon().pixmap(width(), height());
}

void Dockfavmenu::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(title_color, bdr_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawRoundedRect(0, 0, width(), height(), 5, 5);
    painter.drawPixmap(QRect(0, 0, width(), height()), back_pix, QRect(0, 0, back_pix.width(), back_pix.height()));// dock pixmap
    
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    if (zoom)
    {
        painter.drawPixmap(QRect(1, 1, height()-2, height()-2), cat_pix, QRect(0, 0, cat_pix.width(), cat_pix.height()));// category icon
    }
    else
    {
        painter.drawPixmap(QRect(3, 3, height()-6, height()-6), cat_pix, QRect(0, 0, cat_pix.width(), cat_pix.height()));// category icon
    }
}

void Dockfavmenu::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QPoint p = mapToGlobal(QPoint(0, 0));
        
        if (dock_position == 0) // 0 = bottom / 1 = top
            p.setY(p.y()-cat_menu->sizeHint().height());
        else
            p.setY(p.y()+height());
            
        cat_menu->popup(p);
    }
}

void Dockfavmenu::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    zoom = true;
    update();
}

void Dockfavmenu::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    zoom = false;
    update();
}

void Dockfavmenu::update_style()
{
    read_settings();
    update();
}