////////////////////////////////////////
//  File      : deskfile.cpp          //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "deskfile.h"

////////////////////////////////////////

Deskfile::Deskfile(Categorymenu *menu, const QString &file_nm, const QString &file_pth, const QString &pix, QWidget *parent) : QWidget(parent)
{
    cat_menu = menu;
    file_name = file_nm;
    file_path = file_pth;
    d_file_pix = QPixmap(pix);
    read_settings();
    init();
    show();
}

Deskfile::~Deskfile()
{
    delete open_menu;
    delete antico;
    delete delete_file;
    delete cat_menu;
}

void Deskfile::init()
{
    setFixedSize(100, 50);
    zoom = false;
    selected = false;
    setToolTip(file_path + file_name);
    main_menu = new QMenu(this);
    // show the Category apps list for open the file
    open_menu = main_menu->addMenu(QIcon(open_with_pix), tr("Open with"));

    QList <QMenu *> menu_list = cat_menu->get_menus();
    for (int i = 0; i <  menu_list.size(); ++i)
    {
        open_menu->addMenu(menu_list.at(i));
    }

    delete_file = main_menu->addAction(QIcon(delete_link_pix), tr("Delete link"));
    connect(delete_file, SIGNAL(triggered()), this, SLOT(del_file()));
}

void Deskfile::read_settings()
{
    // get style path
    antico = new QSettings(QDir::homePath() + "/.razor/razor.cfg", QSettings::IniFormat, this);
    antico->beginGroup("Style");
    QString stl_name = antico->value("name").toString();
    QString stl_path = antico->value("path").toString();
    antico->endGroup(); //Style
    // get style values
    QSettings *style = new QSettings(stl_path + stl_name, QSettings::IniFormat, this);
    style->beginGroup("Deskfile");
    d_file_col = style->value("name_color").value<QColor>();
    style->endGroup(); //Deskfile
    style->beginGroup("Other");
    delete_link_pix = stl_path + style->value("delete_link_pix").toString();
    open_with_pix = stl_path + style->value("open_with_pix").toString();
    style->endGroup(); //Other
}

void Deskfile::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setWindow(-50, -50, 100, 50);
    painter.setPen(d_file_col);
    
    if (selected)
    {
        painter.drawRoundedRect(-50, -50, width(), height(), 5, 5);
    }

    QString name = QApplication::fontMetrics().elidedText(file_name, Qt::ElideRight, 90); // if file_name is too long, add ... at the end

    painter.setOpacity(0.5);
    painter.setPen(Qt::black);
    painter.drawText(-49, -14, 100, 20, Qt::AlignHCenter, name); // shadow deskfile name
    painter.setOpacity(1);
    painter.setPen(d_file_col);
    painter.drawText(-50, -15, 100, 20, Qt::AlignHCenter, name); // deskfile name
    
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    if (zoom)
    {
        painter.drawPixmap(QRect(-18, -50, 36, 36), d_file_pix, QRect(0, 0, d_file_pix.width(), d_file_pix.height()));// deskfile pix
    }
    else
    {
        painter.drawPixmap(QRect(-16, -50, 32, 32), d_file_pix, QRect(0, 0, d_file_pix.width(), d_file_pix.height()));// deskfile pix
    }
}

void Deskfile::set_selected(bool select)
{
    selected = select;
    update();
}

void Deskfile::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mousepos = event->pos();
        grabMouse(QCursor(Qt::SizeAllCursor));
    }
}

void Deskfile::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p = event->globalPos()-mousepos;
    move(p.x(), p.y());
}

void Deskfile::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    releaseMouse();
    // save new deskfile position on desk writing on razor.cfg
    antico->beginGroup("Desktop");
    antico->beginGroup("File");
    antico->beginGroup(file_name); // File name
    antico->setValue("pos", pos());
    antico->endGroup(); // File name
    antico->endGroup(); // File
    antico->endGroup(); // Desktop
}

void Deskfile::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        antico->beginGroup("Desktop");
        antico->beginGroup("File");
        antico->beginGroup(file_name); // File name
        QString path = antico->value("path").toString();
        QString name = antico->value("name").toString();
        QString exec = antico->value("exec").toString();
        antico->endGroup(); // File name
        antico->endGroup(); // File
        antico->endGroup(); // Desktop

        if (! exec.isEmpty()) // if the preferred app is set
        {
            QStringList param;
            param << path + name;
            QProcess::startDetached(exec, param); //open the file with the preferred application
        }
    }
}

void Deskfile::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    zoom = true;
    update();
}

void Deskfile::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    zoom = false;
    update();
}

void Deskfile::del_file()
{
    emit destroy_deskfile(this);
}

QString Deskfile::get_file_name() const
{
    return file_name;
}

void Deskfile::contextMenuEvent(QContextMenuEvent *event)
{
    cat_menu->set_cmd_arguments(file_path + file_name); // pass the file path+name as argument
    QAction *act = main_menu->exec(event->globalPos());

    if (act != 0 && act != delete_file)
    {
        QString app_path = act->data().toString(); // application path
        QFileInfo info(app_path);
        QString app = info.baseName(); // only the app name without path

        if (app.contains("-"))
        {
            app = app.section('-', 0, 0); // (gimp-2.6 -> gimp)
        }
        Appicon ico;
        QString icon = ico.get_app_icon(app); // application icon
        qDebug() << "App path:" << app << "App icon:" << icon;
        d_file_pix = icon;
        update(); // update the Deskfile icon
        antico->beginGroup("Desktop");
        antico->beginGroup("File");
        antico->beginGroup(file_name); // File name
        antico->setValue("exec", app_path); // set the favorite application path
        antico->setValue("pix", icon); // replace the default pixmap
        antico->endGroup(); // File name
        antico->endGroup(); // File
        antico->endGroup(); // Desktop
    }
}

void Deskfile::update_style()
{
    read_settings();
    update();
}
