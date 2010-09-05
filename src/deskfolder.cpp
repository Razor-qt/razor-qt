////////////////////////////////////////
//  File      : deskfolder.cpp        //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "deskfolder.h"

////////////////////////////////////////

Deskfolder::Deskfolder(Categorymenu *menu, const QString &dir_nm, const QString &dir_pth, const QRect &geo, QWidget *parent) : QWidget(parent)
{
    cat_menu = menu;
    read_settings();
    dir_name = dir_nm;
    dir_path = dir_pth;
    geom = geo;
    init();
    show();
}

Deskfolder::~Deskfolder()
{
    delete antico;
    delete delete_folder;
    delete open_menu;
    delete cat_menu;
}

void Deskfolder::init()
{
    setFixedSize(100, 50);
    zoom = false;
    selected = false;
    setToolTip(dir_path);
    pix = QPixmap(d_folder_pix); // set default pixmap
    main_menu = new QMenu(this);
    // show the Category apps list for open the file
    open_menu = main_menu->addMenu(QIcon(open_with_pix), tr("Open with"));

    QList <QMenu *> menu_list = cat_menu->get_menus();
    for (int i = 0; i <  menu_list.size(); ++i)
    {
        open_menu->addMenu(menu_list.at(i));
    }

    delete_folder = main_menu->addAction(QIcon(delete_link_pix), tr("Delete link"));
    connect(delete_folder, SIGNAL(triggered()), this, SLOT(del_folder()));
}

void Deskfolder::read_settings()
{
    // get style path
    antico = new QSettings(QDir::homePath() + "/.razor/razor.cfg", QSettings::IniFormat, this);
    antico->beginGroup("Style");
    QString stl_name = antico->value("name").toString();
    QString stl_path = antico->value("path").toString();
    antico->endGroup(); //Style
    // get style values
    QSettings *style = new QSettings(stl_path + stl_name, QSettings::IniFormat, this);
    style->beginGroup("Deskfolder");
    d_folder_pix = stl_path + style->value("d_folder_pix").toString();
    d_folder_col = style->value("name_color").value<QColor>();
    geom = style->value("geometry").value<QRect>();
    style->endGroup(); //Deskfolder
    style->beginGroup("Other");
    delete_link_pix = stl_path + style->value("delete_link_pix").toString();
    open_with_pix = stl_path + style->value("open_with_pix").toString();
    style->endGroup(); //Other
}

void Deskfolder::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setWindow(-50, -50, 100, 50);
    painter.setPen(d_folder_col);
        
    if (selected)
    {
        painter.drawRoundedRect(-50, -50, width(), height(), 5, 5);
    }
    
    QString name = QApplication::fontMetrics().elidedText(dir_name, Qt::ElideRight, 90); // if dir_name is too long, add ... at the end

    painter.setOpacity(0.5);
    painter.setPen(Qt::black);
    painter.drawText(-49, -14, 100, 20, Qt::AlignHCenter, name); // shadow deskfolder name
    painter.setOpacity(1);
    painter.setPen(d_folder_col);
    painter.drawText(-50, -15, 100, 20, Qt::AlignHCenter, name); // deskfolder name
    
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
      
    if (zoom)
    {
        painter.drawPixmap(QRect(-18, -50, 36, 36), pix, QRect(0, 0, pix.width(), pix.height()));// deskfolder pix
    }
    else
    {
        painter.drawPixmap(QRect(-16, -50, 32, 32), pix, QRect(0, 0, pix.width(), pix.height()));// deskfolder pix
    }
}

void Deskfolder::set_selected(bool select)
{
    selected = select;
    update();
}

void Deskfolder::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mousepos = event->pos();
        grabMouse(QCursor(Qt::SizeAllCursor));
    }
}

void Deskfolder::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p = event->globalPos()-mousepos;
    move(p.x(), p.y());
}

void Deskfolder::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    releaseMouse();
    // save new deskfolder position on desk writing on razor.cfg
    antico->beginGroup("Desktop");
    antico->beginGroup("Folder");
    antico->beginGroup(dir_name); // Icon name
    antico->setValue("pos", pos());
    antico->endGroup(); // Folder name
    antico->endGroup(); // Folder
    antico->endGroup(); // Desktop
}

void Deskfolder::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        antico->beginGroup("Desktop");
        antico->beginGroup("Folder");
        antico->beginGroup(dir_name); // Folder name
        QString path = antico->value("path").toString();
        QString exec = antico->value("exec").toString();
        antico->endGroup(); // Folder name
        antico->endGroup(); // Folder
        antico->endGroup(); // Desktop

        if (! exec.isEmpty()) // if the preferred app is set
        {
            QStringList param;
            param << path;
            QProcess::startDetached(exec, param); //open the file with the preferred application
        }
        else // open with Filedialog
        {
            Filedialog *file_dialog = new Filedialog(cat_menu);
            file_dialog->set_type(tr("Folder contents:"), "Close");
            file_dialog->setGeometry(geom);
            file_dialog->set_path(dir_path);

            if (file_dialog->exec() == QDialog::Rejected) // on close button
            {
                // save deskicon geometry
                antico->beginGroup("Desktop");
                antico->beginGroup("Folder");
                antico->beginGroup(dir_name);
                antico->setValue("geometry", file_dialog->geometry());
                antico->endGroup(); //name
                antico->endGroup(); //Folder
                antico->endGroup(); //Desktop
                geom = file_dialog->geometry(); // update geometry
            }
        }
    }
}

void Deskfolder::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    zoom = true;
    update();
}

void Deskfolder::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    zoom = false;
    update();
}

void Deskfolder::contextMenuEvent(QContextMenuEvent *event)
{
    cat_menu->set_cmd_arguments(dir_path); // set the dir path as argument
    QAction *act = main_menu->exec(event->globalPos());

    if (act != 0 && act != delete_folder)
    {
        QString app_path = act->data().toString(); // application path
        antico->beginGroup("Desktop");
        antico->beginGroup("Folder");
        antico->beginGroup(dir_name); // Folder name
        antico->setValue("exec", app_path); // set the favorite application path
        antico->endGroup(); // Folder name
        antico->endGroup(); // Folder
        antico->endGroup(); // Desktop
    }
}

void Deskfolder::del_folder()
{
    emit destroy_deskfolder(this);
}

QString Deskfolder::get_dir_name() const
{
    return dir_name;
}

void Deskfolder::update_style()
{
    read_settings();
    pix = QPixmap(d_folder_pix); // set default pixmap
    update();
}
