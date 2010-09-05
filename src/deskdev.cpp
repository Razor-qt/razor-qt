////////////////////////////////////////
//  File      : deskdev.cpp           //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "deskdev.h"

////////////////////////////////////////

Deskdev::Deskdev(Categorymenu *menu, const QString &uu, const QString &device, const QString &mnt_path, const QString &label, const QString &type, Desk *parent) : QWidget(parent)
{
    desktop = parent;
    uuid = uu;
    cat_menu = menu;
    device_name = device;
    mount_path = mnt_path;
    device_label = label;
    device_type = type;
    setFixedSize(100, 50);
    setToolTip(tr("Mount path: ") + mnt_path);
    zoom = false;
    read_settings();
    init();
    show();
}

Deskdev::~Deskdev()
{
    delete open_menu;
    delete cat_menu;
}

void Deskdev::read_settings()
{
    // get style path
    QSettings *antico = new QSettings(QDir::homePath() + "/.razor/razor.cfg", QSettings::IniFormat, this);
    antico->beginGroup("Style");
    QString stl_name = antico->value("name").toString();
    QString stl_path = antico->value("path").toString();
    antico->endGroup(); //Style
    // get style values
    QSettings *style = new QSettings(stl_path + stl_name, QSettings::IniFormat, this);
    style->beginGroup("Deskdev");
    d_disk_pix = stl_path + style->value("d_disk_pix").toString();
    d_cdrom_pix = stl_path + style->value("d_cdrom_pix").toString();
    d_dev_col = style->value("name_color").value<QColor>();
    style->endGroup(); //Deskdev
    style->beginGroup("Other");
    open_with_pix = stl_path + style->value("open_with_pix").toString();
    style->endGroup(); //Other
}

void Deskdev::init()
{
    if (device_type == "disk") // select the right pixmap (cdrom-disk)
        dev_pix = QPixmap(d_disk_pix);
    else // is cdrom
        dev_pix = QPixmap(d_cdrom_pix);

    main_menu = new QMenu(this);
    // show the Category apps list for open the device
    open_menu = main_menu->addMenu(QIcon(open_with_pix), tr("Open with"));

    QList <QMenu *> menu_list = cat_menu->get_menus();
    for (int i = 0; i <  menu_list.size(); ++i)
    {
        open_menu->addMenu(menu_list.at(i));
    }

    main_menu->addAction(QIcon(d_disk_pix), tr("Remove device"));
    connect(main_menu, SIGNAL(triggered(QAction *)), this, SLOT(run_menu(QAction *)));
}

void Deskdev::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setWindow(-50, -50, 100, 50);
    painter.setPen(d_dev_col);

    QString name = QApplication::fontMetrics().elidedText(device_name, Qt::ElideRight, 90); // if device_name is too long, add ... at the end

    painter.setOpacity(0.5);
    painter.setPen(Qt::black);
    painter.drawText(-49, -14, 100, 20, Qt::AlignHCenter, name); // shadow deskdev name
    painter.setOpacity(1);
    painter.setPen(d_dev_col);
    painter.drawText(-50, -15, 100, 20, Qt::AlignHCenter, name); // deskdev name
    
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    if (zoom)
    {
        painter.drawPixmap(QRect(-18, -50, 36, 36), dev_pix, QRect(0, 0, dev_pix.width(), dev_pix.height()));// deskdev pix
    }
    else
    {
        painter.drawPixmap(QRect(-16, -50, 32, 32), dev_pix, QRect(0, 0, dev_pix.width(), dev_pix.height()));// deskdev pix
    }
}

void Deskdev::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mousepos = event->pos();
        grabMouse(QCursor(Qt::SizeAllCursor));
    }
}

void Deskdev::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p = event->globalPos()-mousepos;
    move(p.x(), p.y());
}

void Deskdev::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    releaseMouse();
}

void Deskdev::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        Filedialog *file_dialog = new Filedialog(cat_menu);
        file_dialog->set_type(tr("Device contents:"), "Close");
        file_dialog->set_path(mount_path);
        file_dialog->exec();
    }
}

void Deskdev::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    zoom = true;
    update();
}

void Deskdev::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    zoom = false;
    update();
}

void Deskdev::contextMenuEvent(QContextMenuEvent *event)
{
    cat_menu->set_cmd_arguments(mount_path); // set the mount path as argument
    main_menu->exec(event->globalPos());
}

void Deskdev::run_menu(QAction *act)
{
    if (act->text().compare(tr("Remove device")) == 0)
    {
        desktop->device_removed(uuid);
        qDebug() << "Deskdev removed from menu selection";
    }
}

void Deskdev::update_style()
{
    read_settings();
    if (device_type == "disk") // select the right pixmap (cdrom-disk)
        dev_pix = QPixmap(d_disk_pix);
    else // is cdrom
        dev_pix = QPixmap(d_cdrom_pix);
    update();
}


