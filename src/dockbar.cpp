////////////////////////////////////////
// File : dockbar.cpp //
// Written by: g_cigala@virgilio.it //
// Copyright : GPL //
////////////////////////////////////////

#include "dockbar.h"
////////////////////////////////////////

Dockbar::Dockbar(Razor *a, QWidget *parent) : QLabel(parent)
{
    app = a;
    file_dialog = app->get_file_dialog();
    dock_layout = new QHBoxLayout(this);
    setLayout(dock_layout);
    dock_layout->setContentsMargins(0, 0, 0, 0);
    dock_layout->setSpacing(1);
    dock_layout->setSizeConstraint(QLayout::SetNoConstraint);
    setAcceptDrops(true); // for drag and drop from Filedialog
    setAttribute(Qt::WA_AlwaysShowToolTips);
    read_settings();
    init();
    set_geometry();

    // add launcher to dockbar
    lchr = new Launcher(a, this);
    lchr->setFixedSize(dock_height-1, dock_height-1);
    // for set category menu on dockbar
    //d_menu_widget = new QWidget(this);
    
    // set the favourite-menu
    d_favmenu_widget = new QWidget(this);
    
    // for set dockapp on dockbar
    d_app_widget = new QWidget(this);
    // for set dockicon on dockbar
    d_icon_widget = new QWidget(this);
    // add systray to dockbar
    sys = new Systray(this);
    // add clock to dockbar
    clk = new Dateclock(this);
    clk->setFixedSize(dock_height*2, dock_height-1);

    //menu_layout = new QHBoxLayout();
    //d_menu_widget->setLayout(menu_layout);
    //menu_layout->setAlignment(Qt::AlignLeft);
    //menu_layout->setContentsMargins(0, 0, 0, 0);
    //menu_layout->setSpacing(1);

    favmenu_layout = new QHBoxLayout();
    d_favmenu_widget->setLayout(favmenu_layout);
    favmenu_layout->setAlignment(Qt::AlignLeft);
    favmenu_layout->setContentsMargins(0, 0, 0, 0);
    favmenu_layout->setSpacing(1);
    
    
    
    icon_layout = new QHBoxLayout();
    d_icon_widget->setLayout(icon_layout);
    icon_layout->setAlignment(Qt::AlignLeft);
    icon_layout->setContentsMargins(5, 0, 5, 0);
    icon_layout->setSpacing(1);

    app_layout = new QHBoxLayout();
    d_app_widget->setLayout(app_layout);
    app_layout->setAlignment(Qt::AlignLeft);
    app_layout->setContentsMargins(5, 0, 5, 0);
    app_layout->setSpacing(1);

    dock_layout->insertWidget(0, lchr);
    //dock_layout->insertWidget(1, d_menu_widget, 1);
    dock_layout->insertWidget(1, d_favmenu_widget);
    dock_layout->insertWidget(2, d_app_widget);
    dock_layout->insertWidget(3, d_icon_widget, 8); // max stretch factor
    dock_layout->insertWidget(4, sys,1);
    dock_layout->insertWidget(5, clk);

    //set_dockmenu(); // at startup, restore category menu on dockbar
    set_favmenu();
    set_dockapp(); // at startup, restore dockapps on dockbar
    update_style();
    show();
}

Dockbar::~Dockbar()
{
    delete antico;
    delete dock_layout;
    delete icon_layout;
    delete app_layout;
    delete menu_layout;
    delete d_icon;
    delete clk;
    delete app;
    delete file_dialog;
}

void Dockbar::read_settings()
{
    // get style path
    antico = new QSettings(QDir::homePath() + "/.razor/razor.cfg", QSettings::IniFormat, this);
    antico->beginGroup("Style");
    QString stl_name = antico->value("name").toString();
    QString stl_path = antico->value("path").toString();
    antico->endGroup(); //Style
    // get style values
    //std::cout << stl_name.c_str() << " " << stl_path.c_str() << std::endl;
    QSettings *style = new QSettings(stl_path + stl_name, QSettings::IniFormat, this);
    style->beginGroup("Dockbar");
    dock_pix = stl_path + style->value("dock_pix").toString();
    dock_height = style->value("dock_height").toInt();
    dock_width = style->value("dock_width").toInt(); //why?? just put it to desktop-width?
    
    //
    dock_width = QApplication::desktop()->width();
    //
    dock_position = style->value("dock_position").toInt();
    style->endGroup(); //Dockbar
    style->beginGroup("Other");
    app_link_pix = stl_path + style->value("app_link_pix").toString();
    style->endGroup(); //Other
}

void Dockbar::init()
{
    menu = new QMenu(this);
    menu->addAction(QIcon(app_link_pix), tr("New link to application"));
    connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(run_menu(QAction *)));
}

void Dockbar::set_geometry()
{
    setPixmap(dock_pix);
    setScaledContents(true);

    if (dock_width >= QApplication::desktop()->width())
        dock_width = QApplication::desktop()->width();

    int space_dock = (QApplication::desktop()->width()-dock_width)/2; // space left on right/left side of Dockbar

    if (dock_position == 0) // 0 = bottom / 1 = top
        setGeometry(space_dock, QApplication::desktop()->height()-dock_height, dock_width, dock_height);
    else // top
        setGeometry(space_dock, 0, dock_width, dock_height);
}

void Dockbar::update_dockicon_name(const QString &name, Frame *frm)
{
    if (dock_icons.contains(frm->winId())) // if already present
    {
        Dockicon *d_icon = dock_icons.value(frm->winId());
        d_icon->update_name(name);
    }
}

void Dockbar::add_dockicon(Frame *frm)
{
    if (! dock_icons.contains(frm->winId())) // if not already present
    {
        d_icon = new Dockicon(frm, sys);
        dock_icons.insert(frm->winId(), d_icon); // save the Frame winId/Dockicon
        qDebug() << "Dockicon added to Dockbar. Frame:" << frm->winId();
        update_dockicon_size();
        connect(d_icon, SIGNAL(destroy_dockicon(Dockicon *)), this, SLOT(remove_dockicon(Dockicon *))); // delete iconize dockicon and update dockbar size
    }
}

void Dockbar::remove_dockicon(Frame *frm)
{
    if (dock_icons.contains(frm->winId())) // remove Dockicon if present
    {
        Dockicon *d_icon = dock_icons.value(frm->winId());
        remove_dockicon(d_icon);
    }
}

void Dockbar::remove_dockicon(Dockicon *d_icon) // remove from "Close" right button mouse on Dockbar
{
    dock_icons.remove(dock_icons.key(d_icon));
    qDebug() << "Dockicon remove. Num. after deletion:" << dock_icons.size();
    d_icon->close();
    update_dockicon_size();
}

void Dockbar::remove_dockicon(Window win_id) //remove from "Close" cmd on Systray (_NET_SYSTEM_TRAY protocol) if Dockicon is still mapped
{
    if (dock_icons.contains(win_id))
    {
        Dockicon *d_icon = dock_icons.value(win_id);
        dock_icons.remove(win_id);
        qDebug() << "Dockicon remove from Systray cmd. Num. after deletion:" << dock_icons.size();
        d_icon->close();
        update_dockicon_size();
    }
}

void Dockbar::remove_dockapp(Dockapp *d_app) // remove from "Delete link" right button mouse on Dockbar
{
    dock_apps.removeOne(d_app);
    qDebug() << "Dockapp remove. Num. after deletion:" << dock_apps.size();
    d_app->close();
}

void Dockbar::update_dockicon_size()
{
    if (! dock_icons.isEmpty())
    {
        int num = dock_icons.size();
        qDebug() << "Dockicon num:" << num;
        d_length = d_icon_widget->width()/num;

        if (d_length >= d_icon_widget->width()/3) // max dockicon size = d_icon_widget size/3
            d_length = d_icon_widget->width()/3;

        qDebug() << "Dockicon length:" << d_length;

        foreach(Dockicon *d_icon, dock_icons)
        {
            d_icon->setFixedSize(d_length-2, dock_height-5);
            icon_layout->addWidget(d_icon);
        }
    }
}

void Dockbar::run_menu(QAction *act)
{
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
                create_dock_app(name, path, this);
            }
        }
    }
}

void Dockbar::set_dockapp()
{
    // read dockapp name, path and pixmap and restore on dockbar
    antico->beginGroup("Dockbar");
    antico->beginGroup("App");

    for (int i = 0; i < antico->childGroups().size(); i++)
    {
        antico->beginGroup(antico->childGroups().value(i)); // App name

        QString name = antico->value("name").toString();
        QString exec = antico->value("exec").toString();
        QString pix = antico->value("pix").toString();
        Dockapp *d_app = new Dockapp(name, exec, pix, this);
        dock_apps << d_app; // save the dockapp

        d_app->setFixedSize(dock_height-2, dock_height-2);
        app_layout->addWidget(d_app);

        connect(d_app, SIGNAL(destroy_dockapp(Dockapp *)), this, SLOT(remove_dockapp(Dockapp *))); // delete dockapp form list

        antico->endGroup(); // App name
    }
    antico->endGroup(); //App
    antico->endGroup(); //Dockapp
}

void Dockbar::set_dockmenu()
{
    // add category menu on Dockbar
    QList <QMenu *> menu_list = app->get_category_menu()->get_menus();
    for (int i = 0; i <  menu_list.size(); ++i)
    {
        Dockmenu *cat_menu = new Dockmenu(menu_list.at(i));
        dock_menus << cat_menu; // save the category menu
        cat_menu->setFixedSize(dock_height-2, dock_height-2);
        qDebug()  << "Add category menu on Dockbar:" << menu_list.at(i)->title() ;
        menu_layout->addWidget(cat_menu);
    }
}

void Dockbar::set_favmenu()
{
    // add category menu on Dockbar
    Dockmenu *favmenu = new Dockmenu(app->get_category_menu()->get_favmenu());
    qDebug() << "Added favmenu: "<< app->get_category_menu()->get_favmenu()->title();
    favmenu->setFixedSize(dock_height-2,dock_height-2);
    dock_menus << favmenu; //for space reservation..
    favmenu_layout->addWidget(favmenu);
}


void Dockbar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        menu->exec(event->globalPos());
    }
}

void Dockbar::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "dragEnterEvent";
    event->acceptProposedAction();
}

void Dockbar::dragMoveEvent(QDragMoveEvent *event)
{
    qDebug() << "dragMoveEvent";
    event->acceptProposedAction();
}

void Dockbar::dropEvent(QDropEvent *event) // add apps on Dockbar by drag&drop from Filedialog
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

            if (! fs_model->isDir(index)) // is not a directory
            {
                QString filepath = fs_model->fileInfo(index).filePath();
                QFileInfo pathinfo(filepath);
                QString path = pathinfo.absolutePath(); // remove the file name from path
                path.append("/"); // add slash at the end
                qDebug() << "Selected path:" << path;

                QFileInfo nameinfo(name);
                Fileicon *prov = (Fileicon *)fs_model->iconProvider();
                QString icon = prov->icon_type(nameinfo); // get the file icon

                if (! name.isEmpty() && pathinfo.isExecutable()) // if is an executable
                {
                    create_dock_app(name, path, this);
                }
            }
        }
    }
}

void Dockbar::create_dock_app(const QString &name, const QString &exec, QWidget *parent)
{
    Appicon app_icon; // get application icon
    QString pix = app_icon.get_app_icon(name);
    Dockapp *d_app = new Dockapp(name, exec, pix, parent); // new dockbar application
    dock_apps << d_app; // save the new dockapp
    // save new dockapp name, path and pix
    antico->beginGroup("Dockbar");
    antico->beginGroup("App");
    antico->beginGroup(name);
    antico->setValue("name", name);
    antico->setValue("exec", exec);
    antico->setValue("pix", pix);
    antico->endGroup(); //name
    antico->endGroup(); //App
    antico->endGroup(); //Dockbar

    d_app->setFixedSize(dock_height-2, dock_height-2);
    app_layout->addWidget(d_app);

    connect(d_app, SIGNAL(destroy_dockapp(Dockapp *)), this, SLOT(remove_dockapp(Dockapp *))); // delete dockapp form list
}

void Dockbar::update_style()
{
    read_settings();
    set_geometry();
    update_dockicon_size();
    // update launcher
    lchr->update_style();
    lchr->setFixedSize(dock_height-1, dock_height-1);
    // update dockicons
    foreach(Dockicon *d_icon, dock_icons)
    d_icon->update_style();
    // update category menus
    foreach(Dockmenu *d_menu, dock_menus)
    {
        d_menu->update_style();
        d_menu->setFixedSize(dock_height-2, dock_height-2);
    }
    // update dockapps
    foreach(Dockapp *d_app, dock_apps)
    {
        d_app->update_style();
        d_app->setFixedSize(dock_height-2, dock_height-2);
	qDebug() << d_app->text();
      
    }
    
    
    // update sysicons on systray
    sys->update_style();
    // update dateclock
    clk->update_style();
    clk->setFixedSize(dock_height*2, dock_height-1);
}
