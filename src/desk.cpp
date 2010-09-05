////////////////////////////////////////
//  File      : desk.cpp              //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "desk.h"

////////////////////////////////////////

Desk::Desk(Razor *a, QWidget *parent) : QLabel(parent)
{
    app = a;
    file_dialog = app->get_file_dialog();
    cat_menu = app->get_category_menu();
    read_settings();
    set_geometry();
    set_desk_icons(); // at startup, restore deskfolder/deskfile/deskapp on desktop
    setAcceptDrops(true); // for drag and drop from Filedialog
    setAttribute(Qt::WA_AlwaysShowToolTips);
    init();
    show();
}

Desk::~Desk()
{
    delete antico;
    delete app;
    delete file_dialog;
    delete cat_menu;
}

void Desk::read_settings()
{
    // get style path
    antico = new QSettings(QDir::homePath() + "/.razor/razor.cfg", QSettings::IniFormat, this);
    antico->beginGroup("Style");
    QString stl_name = antico->value("name").toString();
    QString stl_path = antico->value("path").toString();
    antico->endGroup(); //Style
    // get style values
    QSettings *style = new QSettings(stl_path + stl_name, QSettings::IniFormat,this);
    style->beginGroup("Desktop");
    wall_pix = stl_path + style->value("wall_pix").toString();
    style->endGroup(); //Desktop
    style->beginGroup("Dockbar");
    dock_height = style->value("dock_height").toInt();
    style->endGroup(); //Dockbar
    style->beginGroup("Other");
    folder_link_pix = stl_path + style->value("folder_link_pix").toString();
    file_link_pix = stl_path + style->value("file_link_pix").toString();
    app_link_pix = stl_path + style->value("app_link_pix").toString();
    style->endGroup(); //Other
}

void Desk::set_geometry()
{
    setPixmap(wall_pix);
    setScaledContents(true);
    setGeometry(0, 0, QApplication::desktop()->width(), QApplication::desktop()->height()-dock_height);
}

void Desk::set_desk_icons()
{
    // read deskfolder name, path, pix, pos and restore on desktop
    antico->beginGroup("Desktop");
    antico->beginGroup("Folder");

    for (int i = 0; i < antico->childGroups().size(); i++)
    {
        antico->beginGroup(antico->childGroups().value(i)); // Folder name

        QString name = antico->value("name").toString();
        QString path = antico->value("path").toString();
        QRect geom = antico->value("geometry").value<QRect>();
        QPoint pos = antico->value("pos").value<QPoint>();

        d_folder = new Deskfolder(cat_menu, name, path, geom, this);
        desk_folders << d_folder; // save the new deskfolder
        d_folder->move(pos.x(), pos.y());
        connect(d_folder, SIGNAL(destroy_deskfolder(Deskfolder *)), this, SLOT(remove_deskfolder(Deskfolder *))); // delete deskfolder from list

        antico->endGroup(); // Folder name
    }
    antico->endGroup(); //Folder

    // read deskfile name, path, pix, pos and restore on desktop
    antico->beginGroup("File");

    for (int i = 0; i < antico->childGroups().size(); i++)
    {
        antico->beginGroup(antico->childGroups().value(i)); // File name

        QString name = antico->value("name").toString();
        QString path = antico->value("path").toString();
        QString pix = antico->value("pix").toString();
        QPoint pos = antico->value("pos").value<QPoint>();

        d_file = new Deskfile(cat_menu, name, path, pix, this);
        desk_files << d_file; // save the new deskfile
        d_file->move(pos.x(), pos.y());
        connect(d_file, SIGNAL(destroy_deskfile(Deskfile *)), this, SLOT(remove_deskfile(Deskfile *))); // delete deskfile from list

        antico->endGroup(); // File name
    }
    antico->endGroup(); //File

    // read deskapp name, exec, pix, pos and restore on desktop
    antico->beginGroup("App");

    for (int i = 0; i < antico->childGroups().size(); i++)
    {
        antico->beginGroup(antico->childGroups().value(i)); // App name

        QString name = antico->value("name").toString();
        QString exec = antico->value("exec").toString();
        QString pix = antico->value("pix").toString();
        QPoint pos = antico->value("pos").value<QPoint>();

        d_app = new Deskapp(name, exec, pix, this); // new desktop application
        desk_apps << d_app; // save the new deskapp
        d_app->move(pos.x(), pos.y());
        connect(d_app, SIGNAL(destroy_deskapp(Deskapp *)), this, SLOT(remove_deskapp(Deskapp *))); // delete deskapp from list

        antico->endGroup(); // App name
    }
    antico->endGroup(); //App
    antico->endGroup(); //Desktop

    // read Trash and restore on desktop
    antico->beginGroup("Trash");
    QPoint pos = antico->value("pos").value<QPoint>();
    antico->endGroup(); //Trash
    trsh = new Trash(this);
    trsh->move(pos);
}

void Desk::init()
{
    rubber_band = new QRubberBand(QRubberBand::Rectangle, this); // to deskfolder/file/app selection
    menu = new QMenu(this);
    menu->addAction(QIcon(folder_link_pix), tr("New link to folder"));
    menu->addAction(QIcon(file_link_pix), tr("New link to file"));
    menu->addAction(QIcon(app_link_pix), tr("New link to application"));
    connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(run_menu(QAction *)));
    // to mount/unmount external device
    dbus_interface = new QDBusInterface("org.freedesktop.Hal", "/org/freedesktop/Hal/Manager", "org.freedesktop.Hal.Manager", QDBusConnection::systemBus(), this);
    dbus_interface->connection().connect("org.freedesktop.Hal", "/org/freedesktop/Hal/Manager", "org.freedesktop.Hal.Manager", "DeviceAdded", this, SLOT(device_added(const QString &)));
    dbus_interface->connection().connect("org.freedesktop.Hal", "/org/freedesktop/Hal/Manager", "org.freedesktop.Hal.Manager", "DeviceRemoved", this, SLOT(device_removed(const QString &)));
}

void Desk::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        menu->exec(event->pos());
    }
    if (event->button() == Qt::LeftButton)
    {
        foreach(Deskfolder *folder, desk_folders_selected) // unselected previous selected deskfolders
        {
            folder->set_selected(false);
        }
        desk_folders_selected.clear(); // clear previous selected deskfolders

        foreach(Deskfile *file, desk_files_selected) // unselected previous selected deskfiles
        {
            file->set_selected(false);
        }
        desk_files_selected.clear(); // clear previous selected deskfiles

        foreach(Deskapp *app, desk_apps_selected) // unselected previous selected deskapps
        {
            app->set_selected(false);
        }
        desk_apps_selected.clear(); // clear previous selected deskapps

        rubber_press = event->globalPos();
        rubber_band->setGeometry(QRect(rubber_press, QSize()));
        rubber_band->show();
    }
}

void Desk::mouseMoveEvent(QMouseEvent *event)
{
    rubber_band->setGeometry(QRect(rubber_press, event->pos()).normalized());
}

void Desk::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    foreach(Deskfolder *folder, desk_folders) // search selected deskfolders
    {
        if (rubber_band->geometry().intersects(folder->geometry()))
        {
            desk_folders_selected << folder;
            folder->set_selected(true);
        }
    }
    qDebug() << "Deskfolders selected:" << desk_folders_selected.size();

    foreach(Deskfile *file, desk_files) // search selected deskfiles
    {
        if (rubber_band->geometry().intersects(file->geometry()))
        {
            desk_files_selected << file;
            file->set_selected(true);
        }
    }
    qDebug() << "Deskfiles selected:" << desk_files_selected.size();

    foreach(Deskapp *app, desk_apps) // search selected deskapps
    {
        if (rubber_band->geometry().intersects(app->geometry()))
        {
            desk_apps_selected << app;
            app->set_selected(true);
        }
    }
    qDebug() << "Deskapps selected:" << desk_apps_selected.size();

    rubber_band->hide(); // clear rubberband selection
}

void Desk::keyPressEvent(QKeyEvent *event)
{
    if (desk_folders_selected.size() >0 || desk_files_selected.size() >0 || desk_apps_selected.size() >0)
    {
        if (event->key() == Qt::Key_Delete)
        {
            Msgbox msg;
            msg.set_header(tr("DELETE SELECTED ICONS"));
            msg.set_info(tr("Are you sure to delete the selected icons ?"));
            msg.set_icon("Question");

            int ret = msg.exec();

            if (ret == QDialog::Accepted)
            {
                foreach(Deskfolder *folder, desk_folders_selected)
                remove_deskfolder(folder); // remove selected deskfolder
                foreach(Deskfile *file, desk_files_selected)
                remove_deskfile(file); // remove selected deskfile
                foreach(Deskapp *app, desk_apps_selected)
                remove_deskapp(app); // remove selected deskapp
            }
        }
    }
}

void Desk::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "dragEnterEvent";
    qDebug() << "Proposed action:" << event->proposedAction() << " [1:Copy - 2:Move - 4:Link]";
    event->acceptProposedAction();
    qDebug() << "Drag enter contents:" << event->mimeData()->text().toLatin1().data();
}

void Desk::dragMoveEvent(QDragMoveEvent *event)
{
    qDebug() << "dragMoveEvent";
    qDebug() << "Proposed action:" << event->proposedAction() << " [1:Copy - 2:Move - 4:Link]";
    event->acceptProposedAction();
    qDebug() << "Drag move contents:" << event->mimeData()->text().toLatin1().data();
}

void Desk::dropEvent(QDropEvent *event) // add file or directory on desktop by drag&drop from Filedialog
{
    if (event->proposedAction() == Qt::LinkAction)
    {
        qDebug() << "dropEvent";
        QAbstractItemView *abstract_view = (QAbstractItemView *)event->source();
        QFileSystemModel *fs_model = (QFileSystemModel *)abstract_view->model();
        QList<QModelIndex> selection = abstract_view->selectionModel()->selectedIndexes();
        QModelIndex index;

        foreach(index, selection)
        {
            abstract_view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);

            QString name = fs_model->fileInfo(index).fileName();
            qDebug() << "Selected name:" << name;
            QPoint pos = event->pos(); // position of drop event

            if (fs_model->isDir(index)) // is a directory
            {
                QString path = fs_model->fileInfo(index).filePath();
                qDebug() << "Selected path:" << path;
                QRect geometry = abstract_view->geometry(); // get the dimension of View

                if (! name.isEmpty())
                {
                    create_desk_folder(name, path, geometry, pos, this);
                }
            }
            else // is a file
            {
                QString filepath = fs_model->fileInfo(index).filePath();
                QFileInfo pathinfo(filepath);
                QString path = pathinfo.absolutePath(); // remove the file name from path
                path.append("/"); // add slash at the end
                qDebug() << "Selected path:" << path;

                QFileInfo nameinfo(name);
                Fileicon *prov = (Fileicon *)fs_model->iconProvider();
                QString icon = prov->icon_type(nameinfo); // get the file icon

                if (! name.isEmpty())
                {
                    if (pathinfo.isExecutable()) // is an application
                    {
                        Appicon app_icon; // get application icon
                        QString icon = app_icon.get_app_icon(name);
                        create_desk_app(name, path, icon, pos, this);
                    }
                    else // is a file
                    {
                        create_desk_file(name, path, icon, pos, this);
                    }
                }
            }
        }
    }
}

void Desk::run_menu(QAction *act)
{
    if (act->text().compare(tr("New link to folder")) == 0)
    {
        file_dialog->set_type(tr("New link to folder:"), "OK_Close");

        if (file_dialog->exec() == QDialog::Accepted)
        {
            QString path = file_dialog->get_selected_path();
            QString name = file_dialog->get_selected_name();
            QString icon = file_dialog->get_selected_icon();
            QRect geometry = file_dialog->geometry(); // get the dimension of Filedialog
            QPoint pos = menu->pos();

            if (! name.isEmpty() && ! path.endsWith("/")) // is a directory
            {
                create_desk_folder(name, path, geometry, pos, this);
            }
        }
    }
    if (act->text().compare(tr("New link to file")) == 0)
    {
        file_dialog->set_type(tr("New link to file:"), "OK_Close");

        if (file_dialog->exec() == QDialog::Accepted)
        {
            QString path = file_dialog->get_selected_path();
            QString name = file_dialog->get_selected_name();
            QString icon = file_dialog->get_selected_icon();
            QPoint pos = menu->pos();
            QFileInfo pathinfo(path+name);

            if (! name.isEmpty() && pathinfo.isFile() && ! pathinfo.isExecutable())
            {
                create_desk_file(name, path, icon, pos, this);
            }
        }
    }
    if (act->text().compare(tr("New link to application")) == 0)
    {
        file_dialog->set_type(tr("New link to application:"), "OK_Close");

        if (file_dialog->exec() == QDialog::Accepted)
        {
            QString path = file_dialog->get_selected_path();
            QString name = file_dialog->get_selected_name();
            QPoint pos = menu->pos();
            QFileInfo pathinfo(path+name);

            if (! name.isEmpty() && pathinfo.isExecutable())
            {
                Appicon app_icon; // get application icon
                QString icon = app_icon.get_app_icon(name);
                create_desk_app(name, path, icon, pos, this);
            }
        }
    }
}

void Desk::create_desk_folder(const QString &name, const QString &path, const QRect &geometry, const QPoint &pos, QWidget *parent)
{
    d_folder = new Deskfolder(cat_menu, name, path, geometry, parent);
    desk_folders << d_folder; // save the new deskfolder
    d_folder->move(pos.x(), pos.y());
    connect(d_folder, SIGNAL(destroy_deskfolder(Deskfolder *)), this, SLOT(remove_deskfolder(Deskfolder *))); // delete deskfolder from list
    // save new deskfolder name, path, pos and geometry
    antico->beginGroup("Desktop");
    antico->beginGroup("Folder");
    antico->beginGroup(name);
    antico->setValue("name", name);
    antico->setValue("path", path);
    antico->setValue("exec", ""); // for future association with favorite app to open
    antico->setValue("pos", pos);
    antico->setValue("geometry", geometry);
    antico->endGroup(); //name
    antico->endGroup(); //Folder
    antico->endGroup(); //Desktop
}

void Desk::remove_deskfolder(Deskfolder *d_folder) // remove from "Delete link" right button mouse on Desktop and from rubberband selection
{
    // remove the deskfolder from desk and from razor.cfg
    antico->beginGroup("Desktop");
    antico->beginGroup("Folder");
    antico->remove(d_folder->get_dir_name());
    antico->endGroup(); // Folder
    antico->endGroup(); // Desktop
    desk_folders.removeOne(d_folder);
    qDebug() << "Deskfolder remove. Num. after deletion:" << desk_folders.size();
    d_folder->close();
}

void Desk::create_desk_file(const QString &name, const QString &path, const QString &icon, const QPoint &pos, QWidget *parent)
{
    d_file = new Deskfile(cat_menu, name, path, icon, parent);
    desk_files << d_file; // save the new deskfile
    d_file->move(pos.x(), pos.y());
    connect(d_file, SIGNAL(destroy_deskfile(Deskfile *)), this, SLOT(remove_deskfile(Deskfile *))); // delete deskfile from list
    // save new deskfile name, path, icon, exec and position
    antico->beginGroup("Desktop");
    antico->beginGroup("File");
    antico->beginGroup(name);
    antico->setValue("name", name);
    antico->setValue("path", path);
    antico->setValue("pix", icon);
    antico->setValue("exec", ""); // for future association with favorite app to run
    antico->setValue("pos", pos);
    antico->endGroup(); //name
    antico->endGroup(); //File
    antico->endGroup(); //Desktop
}

void Desk::remove_deskfile(Deskfile *d_file) // remove from "Delete link" right button mouse on Desktop and from rubberband selection
{
    // remove the deskicon from desk and from razor.cfg
    antico->beginGroup("Desktop");
    antico->beginGroup("File");
    antico->remove(d_file->get_file_name());
    antico->endGroup(); // File
    antico->endGroup(); // Desktop
    desk_files.removeOne(d_file);
    qDebug() << "Deskfile remove. Num. after deletion:" << desk_files.size();
    d_file->close();
}

void Desk::create_desk_app(const QString &name, const QString &exec, const QString &icon, const QPoint &pos, QWidget *parent)
{
    d_app = new Deskapp(name, exec, icon, parent); // new desktop application
    desk_apps << d_app; // save the new deskapp
    d_app->move(pos.x(), pos.y());
    connect(d_app, SIGNAL(destroy_deskapp(Deskapp *)), this, SLOT(remove_deskapp(Deskapp *))); // delete deskapp from list
    // save new deskapp name, path, pix and pos
    antico->beginGroup("Desktop");
    antico->beginGroup("App");
    antico->beginGroup(name);
    antico->setValue("name", name);
    antico->setValue("exec", exec);
    antico->setValue("pix", icon);
    antico->setValue("pos", pos);
    antico->endGroup(); //name
    antico->endGroup(); //App
    antico->endGroup(); //Desktop
}

void Desk::remove_deskapp(Deskapp *d_app) // remove from "Delete link" right button mouse on Desktop and from rubberband selection
{
    // remove the deskapp from desk and from Antico.conf
    antico->beginGroup("Desktop");
    antico->beginGroup("App");
    antico->remove(d_app->get_app_name());
    antico->endGroup(); // App
    antico->endGroup(); // Desktop
    desk_apps.removeOne(d_app);
    qDebug() << "Deskapp remove. Num. after deletion:" << desk_apps.size();
    d_app->close();
}

void Desk::add_deskicon(Frame *frm)
{
    if (! desk_icons.contains(frm->winId())) // if not already present
    {
        d_icon = new Deskicon(frm);
        desk_icons.insert(frm->winId(), d_icon); // save the Frame winId/Dockicon
        d_icon->move(frm->x() + frm->width()/2 - d_icon->width()/2, frm->y() + frm->height()/2 - d_icon->height()/2); // center in "big" app
        qDebug() << "Deskicon added to Desktop. Frame:" << frm->winId();
        connect(d_icon, SIGNAL(destroy_deskicon(Deskicon *)), this, SLOT(remove_deskicon(Deskicon *))); // delete deskicon
    }
}

void Desk::remove_deskicon(Frame *frm)
{
    if (desk_icons.contains(frm->winId())) // remove Deskicon if present
    {
        Deskicon *d_icon = desk_icons.value(frm->winId());
        remove_deskicon(d_icon);
    }
}

void Desk::remove_deskicon(Deskicon *d_icon) // remove from "Close" right button mouse on Desktop
{
    desk_icons.remove(desk_icons.key(d_icon));
    qDebug() << "Deskicon remove. Num. after deletion:" << desk_icons.size();
    d_icon->close();
}

void Desk::remove_deskicon(Window win_id) //remove from "Close" cmd on Systray (_NET_SYSTEM_TRAY protocol) if Dockicon is still mapped
{
    if (desk_icons.contains(win_id))
    {
        Deskicon *d_icon = desk_icons.value(win_id);
        desk_icons.remove(win_id);
        qDebug() << "Deskicon remove from Systray cmd. Num. after deletion:" << desk_icons.size();
        d_icon->close();
    }
}

void Desk::update_style()
{
    read_settings();
    set_geometry();

    // update deskfolder
    foreach(Deskfolder *folder, desk_folders)
    folder->update_style();
    // update deskfile
    foreach(Deskfile *file, desk_files)
    file->update_style();
    // update deskapp
    foreach(Deskapp *app, desk_apps)
    app->update_style();
    // update deskdev
    foreach(Deskdev *dev, desk_dev)
    dev->update_style();
    // update trash
    trsh->update_style();
}

void Desk::device_added(const QString &uuid)
{
    QString block_device;
    QString vol_label;
    QString vol_fs_type;
    QString drive_type;

    QDBusInterface uuid_interface("org.freedesktop.Hal", uuid, "org.freedesktop.Hal.Device", QDBusConnection::systemBus(), this);
    QDBusReply<bool> is_volume = uuid_interface.call("GetProperty", "block.is_volume");

    qDebug() << "Print no interesting device: " << uuid;

    if (is_volume.isValid() && is_volume.value() == true)
    {
        QDBusReply<QString> fs_usage = uuid_interface.call("GetProperty", "volume.fsusage");
        QDBusReply<bool> ignored = uuid_interface.call("GetProperty", "volume.ignore");

        if (fs_usage.value() == "filesystem" && !ignored.value())
        {
            QDBusReply<QString> block_dev = uuid_interface.call("GetProperty", "block.device");

            if (block_dev.isValid() && !block_dev.value().isEmpty())
                block_device = block_dev;

            QDBusReply<QString> volume_lab = uuid_interface.call("GetProperty", "volume.label");

            if (volume_lab.isValid() && !volume_lab.value().isEmpty())
                vol_label = volume_lab;

            QDBusReply<QString> volume_fs_type = uuid_interface.call("GetProperty", "volume.fstype");

            if (volume_fs_type.isValid() && !volume_fs_type.value().isEmpty())
                vol_fs_type = volume_fs_type.value();

            QDBusReply<QString> storage_device = uuid_interface.call("GetProperty", "block.storage_device");
            QDBusInterface storage_interface("org.freedesktop.Hal", storage_device.value(), "org.freedesktop.Hal.Device",
                                             QDBusConnection::systemBus(), this);

            QDBusReply<QString> drv_type = storage_interface.call("GetProperty", "storage.drive_type");

            if (drv_type.isValid() && !drv_type.value().isEmpty())
                drive_type = drv_type.value();

            QDBusReply<QMap<QString, QVariant> > props = storage_interface.call("GetAllProperties");
            QMapIterator<QString, QVariant> iter(props.value());

            while (iter.hasNext())
            {
                iter.next();
                qDebug() << "Property:" << iter.key() << iter.value(); // print all properties
            }

            qDebug() << "Deskdev added. Block device:" << block_device << "Volume label:" << vol_label << "Volume fstype:"
            << vol_fs_type << "Drive type:" << drive_type << "UUID:" << uuid;

            mount_device(uuid, block_device, vol_label, drive_type, vol_fs_type);
        }
    }
}

void Desk::mount_device(const QString &uuid, const QString &block_device, const QString &vol_label,
                        const QString &drive_type, const QString &vol_fs_type)
{
    QStringList options;
    QString mnt_path = ""; // default is /media

    QDBusInterface uuid_interface("org.freedesktop.Hal", uuid, "org.freedesktop.Hal.Device", QDBusConnection::systemBus(), this);
    QDBusInterface mount_interface("org.freedesktop.Hal", uuid, "org.freedesktop.Hal.Device.Volume",
                                   QDBusConnection::systemBus(), this);

    // mount the device in /media/xxx
    QDBusReply<int> mnt_cmd = mount_interface.call("Mount", mnt_path, vol_fs_type, options);
    // get the mount point
    QDBusReply<QString> mnt_point = uuid_interface.call("GetProperty", "volume.mount_point");
    QString mnt_dir = mnt_point.value();

    Deskdev *d_dev = new Deskdev(cat_menu, uuid, block_device, mnt_dir, vol_label, drive_type, this); // save the new deskdev
    desk_dev.insert(uuid, d_dev);

    if (mnt_cmd)
    {
        Msgbox info;
        info.set_header(tr("WARNING"));
        info.set_info(tr("<b>Problem to mount the device</b>"));
        info.set_icon("Warning");
        info.exec();
    }
    else
    {
        QDBusReply<QString> mnt_point = uuid_interface.call("GetProperty", "volume.mount_point");
        Msgbox info;
        info.set_header(tr("Device mounted in:"));
        info.set_info("<b>" + mnt_dir + "</b>");
        info.set_icon("Information");
        info.exec();
    }
}

void Desk::device_removed(const QString &uuid)
{
    if (desk_dev.contains(uuid))
    {
        qDebug() << "Deskdev removed. UUID:" << uuid;
        Deskdev *d_dev = desk_dev.value(uuid);
        d_dev->close(); // remove the Deskdev icon from Desktop
        desk_dev.remove(uuid); // delete the Deskdev
        unmount_device(uuid);
    }
}

void Desk::unmount_device(const QString &uuid)
{
    QStringList options;
    QString drive_type;
    QDBusInterface uuid_interface("org.freedesktop.Hal", uuid, "org.freedesktop.Hal.Device", QDBusConnection::systemBus(), this);
    QDBusInterface unmount_interface("org.freedesktop.Hal", uuid, "org.freedesktop.Hal.Device.Volume",
                                     QDBusConnection::systemBus(), this);

    // unmount and (if CDROM) eject the device
    QDBusReply<int> unmnt_cmd = unmount_interface.call("Eject", options);

    if (unmnt_cmd)
    {
        Msgbox info;
        info.set_header(tr("WARNING"));
        info.set_info(tr("<b>Problem to unmount the device</b>"));
        info.set_icon("Warning");
        info.exec();
    }
    else
    {
        Msgbox info;
        info.set_header(tr("INFORMATION"));
        info.set_info(tr("<b>Device correctly unmounted</b>"));
        info.set_icon("Information");
        info.exec();
    }
}


