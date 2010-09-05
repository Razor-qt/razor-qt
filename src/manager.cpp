////////////////////////////////////////
//  File      : manager.cpp           //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "manager.h"

////////////////////////////////////////

Manager::Manager(QWidget *parent) : QDialog(parent)
{
    // get style path
    antico = new QSettings(QDir::homePath() + "/.razor/razor.cfg", QSettings::IniFormat, this);
    antico->beginGroup("Style");
    stl_name = antico->value("name").toString();
    stl_path = antico->value("path").toString();
    antico->endGroup(); //Style
    // get style values
    style = new QSettings(stl_path + stl_name, QSettings::IniFormat, this);
    init(); // before that read_settings
    read_settings();
    setSizeGripEnabled(true);
    show();
}

Manager::~Manager()
{}

void Manager::init()
{
    tab = new QTabWidget(this);
    main_layout = new QVBoxLayout(this);
    setLayout(main_layout);
    QLabel *lab = new QLabel(tr("MANAGER"), this);
    lab->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    file_dialog = new Filedialog(this); // without category menu
    main_layout->addWidget(lab);
    main_layout->addWidget(tab);
    style_tab();
    add_app_tab();
    remove_app_tab();
    run_app_tab();
}

void Manager::read_settings()
{
    style_sel_tx->setText(stl_name); // show actual style name
    /////////////////////////////////////////////////////////////////////////
    style->beginGroup("Frame");
    style->beginGroup("Border");
    top_bdr_spinBox->setValue(style->value("top_bdr_height").toInt());
    lateral_bdr_spinBox->setValue(style->value("lateral_bdr_width").toInt());
    bottom_bdr_spinBox->setValue(style->value("bottom_bdr_height").toInt());
    style->endGroup(); // Border
    style->beginGroup("Header");
    header_active_pix_path = stl_path + style->value("header_active_pix").toString();
    header_inactive_pix_path = stl_path + style->value("header_inactive_pix").toString();
    header_active_pix->setToolTip(style->value("header_active_pix").toString());
    header_inactive_pix->setToolTip(style->value("header_inactive_pix").toString());
    header_active_pix->setPixmap(QPixmap(header_active_pix_path));
    header_inactive_pix->setPixmap(QPixmap(header_inactive_pix_path));
    minmax_pix_path = stl_path + style->value("minmax_pix").toString();
    minmax_pix->setToolTip(style->value("minmax_pix").toString());
    minmax_pix->setPixmap(QPixmap(minmax_pix_path));
    close_pix_path = stl_path + style->value("close_pix").toString();
    close_pix->setToolTip(style->value("close_pix").toString());
    close_pix->setPixmap(QPixmap(close_pix_path));
    header_col_lab->setPalette(QPalette(style->value("title_color").value<QColor>()));
    header_col_lab->setAutoFillBackground(true);
    style->endGroup(); //Header
    style->endGroup(); //Frame
    /////////////////////////////////////////////////////////////////////////
    style->beginGroup("Dockbar");
    dockbar_pix_path = stl_path + style->value("dock_pix").toString();
    dockbar_pix->setToolTip(style->value("dock_pix").toString());
    dockbar_pix->setPixmap(QPixmap(dockbar_pix_path));
    dockbar_height_spinBox->setValue(style->value("dock_height").toInt());
    dockbar_width_spinBox->setValue(style->value("dock_width").toInt());
    dockbar_position_comboBox->setCurrentIndex(style->value("dock_position").toInt());
    style->endGroup(); // Dockbar
    /////////////////////////////////////////////////////////////////////////
    style->beginGroup("Dockicon");
    dockicon_pix_path = stl_path + style->value("d_icon_pix").toString();
    dockicon_pix->setToolTip(style->value("d_icon_pix").toString());
    dockicon_pix->setPixmap(QPixmap(dockicon_pix_path));
    dockicon_col_lab->setPalette(QPalette(style->value("title_color").value<QColor>()));
    dockicon_col_lab->setAutoFillBackground(true);
    style->endGroup(); // Dockicon
    /////////////////////////////////////////////////////////////////////////
    style->beginGroup("Deskapp");
    deskapp_col_lab->setPalette(QPalette(style->value("name_color").value<QColor>()));
    deskapp_col_lab->setAutoFillBackground(true);
    style->endGroup(); // Deskapp
    /////////////////////////////////////////////////////////////////////////
    style->beginGroup("Sysicon");
    sysicon_pix_path = stl_path + style->value("s_icon_pix").toString();
    sysicon_pix->setToolTip(style->value("s_icon_pix").toString());
    sysicon_pix->setPixmap(QPixmap(sysicon_pix_path));
    style->endGroup(); // Sysicon
    /////////////////////////////////////////////////////////////////////////
    style->beginGroup("Desktop");
    desktop_pix_path = stl_path + style->value("wall_pix").toString();
    desktop_pix->setToolTip(style->value("wall_pix").toString());
    desktop_pix->setPixmap(QPixmap(desktop_pix_path));
    style->endGroup(); // Desktop
    /////////////////////////////////////////////////////////////////////////
    style->beginGroup("Deskfolder");
    deskfolder_pix_path = stl_path + style->value("d_folder_pix").toString();
    deskfolder_pix->setToolTip(style->value("d_folder_pix").toString());
    deskfolder_pix->setPixmap(QPixmap(deskfolder_pix_path));
    deskfolder_col_lab->setPalette(QPalette(style->value("name_color").value<QColor>()));
    deskfolder_col_lab->setAutoFillBackground(true);
    style->endGroup(); // Deskfolder
    /////////////////////////////////////////////////////////////////////////
    style->beginGroup("Trash");
    trash_pix_path = stl_path + style->value("trash_pix").toString();
    trash_pix->setToolTip(style->value("trash_pix").toString());
    trash_pix->setPixmap(QPixmap(trash_pix_path));
    trash_col_lab->setPalette(QPalette(style->value("name_color").value<QColor>()));
    trash_col_lab->setAutoFillBackground(true);
    style->endGroup(); // Trash
    /////////////////////////////////////////////////////////////////////////
    style->beginGroup("Deskfile");
    deskfile_col_lab->setPalette(QPalette(style->value("name_color").value<QColor>()));
    deskfile_col_lab->setAutoFillBackground(true);
    style->endGroup(); // Deskfile
    /////////////////////////////////////////////////////////////////////////
    style->beginGroup("Deskdev");
    deskdev_disk_pix_path = stl_path + style->value("d_disk_pix").toString();
    deskdev_cdrom_pix_path = stl_path + style->value("d_cdrom_pix").toString();
    deskdev_disk_pix->setToolTip(style->value("d_disk_pix").toString());
    deskdev_cdrom_pix->setToolTip(style->value("d_cdrom_pix").toString());
    deskdev_disk_pix->setPixmap(QPixmap(deskdev_disk_pix_path));
    deskdev_cdrom_pix->setPixmap(QPixmap(deskdev_cdrom_pix_path));
    deskdev_col_lab->setPalette(QPalette(style->value("name_color").value<QColor>()));
    deskdev_col_lab->setAutoFillBackground(true);
    style->endGroup(); // Deskdev
    /////////////////////////////////////////////////////////////////////////
    style->beginGroup("Dateclock");
    date_col_lab->setPalette(QPalette(style->value("date_color").value<QColor>()));
    date_col_lab->setAutoFillBackground(true);
    clock_col_lab->setPalette(QPalette(style->value("clock_color").value<QColor>()));
    clock_col_lab->setAutoFillBackground(true);
    style->endGroup(); // Dateclock
    /////////////////////////////////////////////////////////////////////////
    style->beginGroup("Launcher");
    launcher_pix_path = stl_path + style->value("launcher_pix").toString();
    quit_pix_path = stl_path + style->value("quit_pix").toString();
    shutdown_pix_path = stl_path + style->value("shutdown_pix").toString();
    restart_pix_path = stl_path + style->value("restart_pix").toString();
    refresh_pix_path = stl_path + style->value("refresh_pix").toString();
    run_pix_path = stl_path + style->value("run_pix").toString();
    show_pix_path = stl_path + style->value("show_pix").toString();
    manager_pix_path = stl_path + style->value("manager_pix").toString();
    settings_pix_path = stl_path + style->value("settings_pix").toString();
    utility_pix_path = stl_path + style->value("utility_pix").toString();
    office_pix_path = stl_path + style->value("office_pix").toString();
    network_pix_path = stl_path + style->value("network_pix").toString();
    graphics_pix_path = stl_path + style->value("graphics_pix").toString();
    development_pix_path = stl_path + style->value("development_pix").toString();
    system_pix_path = stl_path + style->value("system_pix").toString();
    audiovideo_pix_path = stl_path + style->value("audiovideo_pix").toString();
    launcher_pix->setToolTip(style->value("launcher_pix").toString());
    quit_pix->setToolTip(style->value("quit_pix").toString());
    shutdown_pix->setToolTip(style->value("shutdown_pix").toString());
    restart_pix->setToolTip(style->value("restart_pix").toString());
    refresh_pix->setToolTip(style->value("refresh_pix").toString());
    run_pix->setToolTip(style->value("run_pix").toString());
    show_pix->setToolTip(style->value("show_pix").toString());
    manager_pix->setToolTip(style->value("manager_pix").toString());
    settings_pix->setToolTip(style->value("settings_pix").toString());
    utility_pix->setToolTip(style->value("utility_pix").toString());
    office_pix->setToolTip(style->value("office_pix").toString());
    network_pix->setToolTip(style->value("network_pix").toString());
    graphics_pix->setToolTip(style->value("graphics_pix").toString());
    development_pix->setToolTip(style->value("development_pix").toString());
    system_pix->setToolTip(style->value("system_pix").toString());
    audiovideo_pix->setToolTip(style->value("audiovideo_pix").toString());
    launcher_pix->setPixmap(QPixmap(launcher_pix_path));
    quit_pix->setPixmap(QPixmap(quit_pix_path));
    shutdown_pix->setPixmap(QPixmap(shutdown_pix_path));
    restart_pix->setPixmap(QPixmap(restart_pix_path));
    refresh_pix->setPixmap(QPixmap(refresh_pix_path));
    run_pix->setPixmap(QPixmap(run_pix_path));
    show_pix->setPixmap(QPixmap(show_pix_path));
    manager_pix->setPixmap(QPixmap(manager_pix_path));
    settings_pix->setPixmap(QPixmap(settings_pix_path));
    utility_pix->setPixmap(QPixmap(utility_pix_path));
    office_pix->setPixmap(QPixmap(office_pix_path));
    network_pix->setPixmap(QPixmap(network_pix_path));
    graphics_pix->setPixmap(QPixmap(graphics_pix_path));
    development_pix->setPixmap(QPixmap(development_pix_path));
    system_pix->setPixmap(QPixmap(system_pix_path));
    audiovideo_pix->setPixmap(QPixmap(audiovideo_pix_path));
    style->endGroup(); // Launcher
    /////////////////////////////////////////////////////////////////////////
    style->beginGroup("Message");
    ok_button_pix_path = stl_path + style->value("ok_button_pix").toString();
    close_button_pix_path = stl_path + style->value("close_button_pix").toString();
    add_button_pix_path = stl_path + style->value("add_button_pix").toString();
    remove_button_pix_path = stl_path + style->value("remove_button_pix").toString();
    restore_button_pix_path = stl_path + style->value("restore_button_pix").toString();
    question_pix_path = stl_path + style->value("question_pix").toString();
    information_pix_path = stl_path + style->value("information_pix").toString();
    warning_pix_path = stl_path + style->value("warning_pix").toString();
    critical_pix_path = stl_path + style->value("critical_pix").toString();
    ok_button_pix->setToolTip(style->value("ok_button_pix").toString());
    close_button_pix->setToolTip(style->value("close_button_pix").toString());
    add_button_pix->setToolTip(style->value("add_button_pix").toString());
    remove_button_pix->setToolTip(style->value("remove_button_pix").toString());
    restore_button_pix->setToolTip(style->value("restore_button_pix").toString());
    question_pix->setToolTip(style->value("question_pix").toString());
    information_pix->setToolTip(style->value("information_pix").toString());
    warning_pix->setToolTip(style->value("warning_pix").toString());
    critical_pix->setToolTip(style->value("critical_pix").toString());
    ok_button_pix->setPixmap(QPixmap(ok_button_pix_path));
    close_button_pix->setPixmap(QPixmap(close_button_pix_path));
    add_button_pix->setPixmap(QPixmap(add_button_pix_path));
    remove_button_pix->setPixmap(QPixmap(remove_button_pix_path));
    restore_button_pix->setPixmap(QPixmap(restore_button_pix_path));
    question_pix->setPixmap(QPixmap(question_pix_path));
    information_pix->setPixmap(QPixmap(information_pix_path));
    warning_pix->setPixmap(QPixmap(warning_pix_path));
    critical_pix->setPixmap(QPixmap(critical_pix_path));
    style->endGroup(); // Message
    /////////////////////////////////////////////////////////////////////////
    style->beginGroup("Other");
    application_pix_path = stl_path + style->value("application_pix").toString();
    folder_link_pix_path = stl_path + style->value("folder_link_pix").toString();
    file_link_pix_path = stl_path + style->value("file_link_pix").toString();
    app_link_pix_path = stl_path + style->value("app_link_pix").toString();
    delete_link_pix_path = stl_path + style->value("delete_link_pix").toString();
    delete_file_pix_path = stl_path + style->value("delete_file_pix").toString();
    cut_file_pix_path = stl_path + style->value("cut_file_pix").toString();
    copy_file_pix_path = stl_path + style->value("copy_file_pix").toString();
    paste_file_pix_path = stl_path + style->value("paste_file_pix").toString();
    close_dock_pix_path = stl_path + style->value("close_dock_pix").toString();
    add_to_sys_pix_path = stl_path + style->value("add_to_sys_pix").toString();
    open_with_pix_path = stl_path + style->value("open_with_pix").toString();
    list_view_pix_path = stl_path + style->value("list_view_pix").toString();
    icon_view_pix_path = stl_path + style->value("icon_view_pix").toString();
    upper_dir_pix_path = stl_path + style->value("upper_dir_pix").toString();
    application_pix->setToolTip(style->value("application_pix").toString());
    folder_link_pix->setToolTip(style->value("folder_link_pix").toString());
    file_link_pix->setToolTip(style->value("file_link_pix").toString());
    app_link_pix->setToolTip(style->value("app_link_pix").toString());
    delete_link_pix->setToolTip(style->value("delete_link_pix").toString());
    delete_file_pix->setToolTip(style->value("delete_file_pix").toString());
    cut_file_pix->setToolTip(style->value("cut_file_pix").toString());
    copy_file_pix->setToolTip(style->value("copy_file_pix").toString());
    paste_file_pix->setToolTip(style->value("paste_file_pix").toString());
    close_dock_pix->setToolTip(style->value("close_dock_pix").toString());
    add_to_sys_pix->setToolTip(style->value("add_to_sys_pix").toString());
    open_with_pix->setToolTip(style->value("open_with_pix").toString());
    list_view_pix->setToolTip(style->value("list_view_pix").toString());
    icon_view_pix->setToolTip(style->value("icon_view_pix").toString());
    upper_dir_pix->setToolTip(style->value("upper_dir_pix").toString());
    application_pix->setPixmap(QPixmap(application_pix_path));
    folder_link_pix->setPixmap(QPixmap(folder_link_pix_path));
    file_link_pix->setPixmap(QPixmap(file_link_pix_path));
    app_link_pix->setPixmap(QPixmap(app_link_pix_path));
    delete_link_pix->setPixmap(QPixmap(delete_link_pix_path));
    delete_file_pix->setPixmap(QPixmap(delete_file_pix_path));
    cut_file_pix->setPixmap(QPixmap(cut_file_pix_path));
    copy_file_pix->setPixmap(QPixmap(copy_file_pix_path));
    paste_file_pix->setPixmap(QPixmap(paste_file_pix_path));
    close_dock_pix->setPixmap(QPixmap(close_dock_pix_path));
    add_to_sys_pix->setPixmap(QPixmap(add_to_sys_pix_path));
    open_with_pix->setPixmap(QPixmap(open_with_pix_path));
    list_view_pix->setPixmap(QPixmap(list_view_pix_path));
    icon_view_pix->setPixmap(QPixmap(icon_view_pix_path));
    upper_dir_pix->setPixmap(QPixmap(upper_dir_pix_path));
    style->endGroup(); //Other
    /////////////////////////////////////////////////////////////////////////
}

void Manager::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::darkGray, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawRect(0, 0, width(), height());
}

void Manager::add_app_tab()
{
    style->beginGroup("Deskfolder");
    deskfolder_pix_path = stl_path + style->value("d_folder_pix").toString();
    style->endGroup(); // Deskfolder
    style->beginGroup("Message");
    add_button_pix_path = stl_path + style->value("add_button_pix").toString();
    style->endGroup(); // Message
    QFrame *add_frm = new QFrame(this);
    tab->addTab(add_frm, QIcon(add_button_pix_path), tr("Add application"));
    add_layout = new QVBoxLayout();
    add_frm->setLayout(add_layout);
    app_path = new QLineEdit(this); // show selection path
    fs_model = new QFileSystemModel(this);
    fs_model->setRootPath("/"); // must set on init
    prov = new Fileicon(); // get the files icon
    fs_model->setIconProvider(prov);
    QCompleter *completer = new QCompleter(this); // to complete the path
    completer->setModel(new QDirModel(completer));
    app_path->setCompleter(completer);
    tree_view = new QTreeView(this);
    tree_view->setModel(fs_model);
    tree_view->setSortingEnabled(true);
    tree_view->setAlternatingRowColors(true);
    tree_view->setFocusPolicy(Qt::ClickFocus);
    QSplitter *splitter = new QSplitter(this);
    path_widget = new QListWidget(this);
    path_widget->setMaximumWidth(150);
    root_item = new QListWidgetItem(path_widget);
    bin_item = new QListWidgetItem(path_widget);
    home_item = new QListWidgetItem(path_widget);
    root_item->setIcon(QIcon(deskfolder_pix_path));
    root_item->setText(tr("/"));
    bin_item->setIcon(QIcon(deskfolder_pix_path));
    bin_item->setText(tr("/usr/bin/"));
    home_item->setIcon(QIcon(deskfolder_pix_path));
    home_item->setText(tr("/home/"));
    splitter->addWidget(path_widget);
    splitter->addWidget(tree_view);
    add_layout->addWidget(app_path);
    add_layout->addWidget(splitter);
    category_lay = new QHBoxLayout();
    add_layout->addLayout(category_lay);
    QLabel *category_lab = new QLabel(tr("Select the category:"), this);
    category_combo = new QComboBox(this);
    // add Category with icon on Combobox (if new Category is added, remember to update Launcher menu)
    style->beginGroup("Launcher");
    category_combo->addItem(QIcon(stl_path + style->value("utility_pix").toString()), tr("Utility"));
    category_combo->addItem(QIcon(stl_path + style->value("office_pix").toString()), tr("Office"));
    category_combo->addItem(QIcon(stl_path + style->value("network_pix").toString()), tr("Network"));
    category_combo->addItem(QIcon(stl_path + style->value("graphics_pix").toString()), tr("Graphics"));
    category_combo->addItem(QIcon(stl_path + style->value("development_pix").toString()), tr("Development"));
    category_combo->addItem(QIcon(stl_path + style->value("system_pix").toString()), tr("System"));
    category_combo->addItem(QIcon(stl_path + style->value("audiovideo_pix").toString()), tr("AudioVideo"));
    category_combo->addItem(QIcon(stl_path + style->value("favourite_pix").toString()), tr("Favourite"));
    style->endGroup(); // Launcher
    // to map translated name with native categories
    cat_map.insert(tr("Utility"), "Utility");
    cat_map.insert(tr("Office"), "Office");
    cat_map.insert(tr("Network"), "Network");
    cat_map.insert(tr("Graphics"), "Graphics");
    cat_map.insert(tr("Development"), "Development");
    cat_map.insert(tr("System"), "System");
    cat_map.insert(tr("AudioVideo"), "AudioVideo");
    cat_map.insert(tr("Favourite"), "Favourite");
    style->beginGroup("Message");
    QPushButton *add_but = new QPushButton(QIcon(stl_path + style->value("add_button_pix").toString()), tr("Add"), this);
    QPushButton* close_but = new QPushButton(QIcon(stl_path + style->value("close_button_pix").toString()), tr("Close"), this);
    style->endGroup(); // Message
    category_lay->addStretch(1);
    category_lay->addWidget(category_lab);
    category_lay->addWidget(category_combo);
    category_lay->addWidget(add_but);
    category_lay->addWidget(close_but);

    connect(path_widget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this, SLOT(change_path(QListWidgetItem *, QListWidgetItem *)));
    connect(tree_view, SIGNAL(clicked(QModelIndex)), this, SLOT(show_path(QModelIndex)));
    connect(add_but, SIGNAL(pressed()), this, SLOT(add_app_pressed()));
    connect(close_but, SIGNAL(pressed()), this, SLOT(close_pressed()));
    connect(app_path, SIGNAL(returnPressed()), this, SLOT(path_completer()));
}

void Manager::remove_app_tab()
{
    style->beginGroup("Message");
    remove_button_pix_path = stl_path + style->value("remove_button_pix").toString();
    style->endGroup(); // Message
    QFrame *rem_frm = new QFrame(this);
    tab->addTab(rem_frm, QIcon(remove_button_pix_path), tr("Remove application"));
    rem_layout = new QVBoxLayout();
    rem_frm->setLayout(rem_layout);

    app_tree = new QTreeWidget(this);
    app_tree->setColumnCount(1);
    app_tree->setHeaderLabel(tr("Category/Applications"));
    app_tree->setAlternatingRowColors(true);
    app_tree->setFocusPolicy(Qt::ClickFocus);
    update_remove_list();

    QHBoxLayout *rem_close_layout = new QHBoxLayout();
    style->beginGroup("Message");
    QPushButton *rem_but = new QPushButton(QIcon(stl_path + style->value("remove_button_pix").toString()), tr("Remove"), this);
    QPushButton* close_but = new QPushButton(QIcon(stl_path + style->value("close_button_pix").toString()), tr("Close"), this);
    style->endGroup(); // Message
    rem_close_layout->addStretch(1);
    rem_close_layout->addWidget(rem_but);
    rem_close_layout->addWidget(close_but);

    rem_layout->addWidget(app_tree);
    rem_layout->addLayout(rem_close_layout);

    connect(rem_but, SIGNAL(pressed()), this, SLOT(remove_app_pressed()));
    connect(close_but, SIGNAL(pressed()), this, SLOT(close_pressed()));
}

void Manager::run_app_tab()
{
    style->beginGroup("Other");
    application_pix_path = stl_path + style->value("application_pix").toString();
    style->endGroup(); // Other
    QFrame *run_frm = new QFrame(this);
    tab->addTab(run_frm, QIcon(application_pix_path), tr("Run at startup"));
    run_layout = new QVBoxLayout();
    run_frm->setLayout(run_layout);

    run_table = new QTableWidget(0, 3, this);
    QStringList header;
    header << tr("Application") << tr("Path") << tr("Arguments");
    run_table->setHorizontalHeaderLabels(header);
    run_table->setAlternatingRowColors(true);
    update_run_table();

    QHBoxLayout *add_rem_layout = new QHBoxLayout();
    style->beginGroup("Message");
    QPushButton *add_but = new QPushButton(QIcon(stl_path + style->value("add_button_pix").toString()), tr("Add"), this);
    QPushButton *rem_but = new QPushButton(QIcon(stl_path + style->value("remove_button_pix").toString()), tr("Remove"), this);
    QPushButton* close_but = new QPushButton(QIcon(stl_path + style->value("close_button_pix").toString()), tr("Close"), this);
    style->endGroup(); // Message

    add_rem_layout->addStretch(1);
    add_rem_layout->addWidget(add_but);
    add_rem_layout->addWidget(rem_but);
    add_rem_layout->addWidget(close_but);

    run_layout->addWidget(run_table);
    run_layout->addLayout(add_rem_layout);

    connect(run_table, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(set_run_args(QTableWidgetItem *))); // to set run args for application
    connect(add_but, SIGNAL(pressed()), this, SLOT(add_run_app_pressed()));
    connect(rem_but, SIGNAL(pressed()), this, SLOT(remove_run_app_pressed()));
    connect(close_but, SIGNAL(pressed()), this, SLOT(close_pressed()));
}

void Manager::style_tab()
{
    style->beginGroup("Launcher");
    settings_pix_path = stl_path + style->value("settings_pix").toString();
    style->endGroup(); // Launcher
    QFrame *style_frm = new QFrame(this);
    tab->addTab(style_frm, QIcon(settings_pix_path), tr("Style"));
    style_layout = new QVBoxLayout();
    style_frm->setLayout(style_layout);
    ///////// STYLE SELECTION /////////
    style_grid = new QGridLayout();
    style_grid->setSpacing(2);
    style_layout->addLayout(style_grid);
    QLabel *style_sel_lb = new QLabel(tr("Actual style:"), this);
    style_sel_tx = new QLineEdit(this);
    style_sel_tx->setReadOnly(true);
    QPushButton *style_sel_but = new QPushButton("...", this);
    style_sel_but->setMaximumWidth(50);
    style_grid->addWidget(style_sel_lb, 0, 0);
    style_grid->addWidget(style_sel_tx, 0, 1);
    style_grid->addWidget(style_sel_but, 0, 2);
    connect(style_sel_but, SIGNAL(clicked()), this, SLOT(select_style()));
    /////////////////////////////////////////////////////
    sections_widget = new QListWidget(this);
    sections_widget->setMaximumWidth(200);
    sections_widget->setCurrentRow(0);
    QListWidgetItem *frame_section = new QListWidgetItem(sections_widget);
    frame_section->setText(tr("Frame"));
    QListWidgetItem *dockbar_section = new QListWidgetItem(sections_widget);
    dockbar_section->setText(tr("Dockbar"));
    QListWidgetItem *desktop_section = new QListWidgetItem(sections_widget);
    desktop_section->setText(tr("Desktop"));
    QListWidgetItem *wallpaper_section = new QListWidgetItem(sections_widget);
    wallpaper_section->setText(tr("Wallpaper"));
    QListWidgetItem *launcher_section = new QListWidgetItem(sections_widget);
    launcher_section->setText(tr("Launcher"));
    QListWidgetItem *category_section = new QListWidgetItem(sections_widget);
    category_section->setText(tr("Categories"));
    QListWidgetItem *message_section = new QListWidgetItem(sections_widget);
    message_section->setText(tr("Message"));
    QListWidgetItem *other_section = new QListWidgetItem(sections_widget);
    other_section->setText(tr("Other"));
    params_widget = new QStackedWidget(this);
    QSplitter *splitter = new QSplitter(this);
    splitter->addWidget(sections_widget);
    splitter->addWidget(params_widget);
    style_layout->addWidget(splitter);
    ///////// FRAME /////////
    frame_box = new QGroupBox(tr("Frame"), this);
    frame_grid = new QGridLayout();
    frame_grid->setSpacing(2);
    frame_box->setLayout(frame_grid);
    QLabel *top_bdr_lb = new QLabel(tr("Top border height:"), this);
    QLabel *lateral_bdr_lb = new QLabel(tr("Lateral border width:"), this);
    QLabel *bottom_bdr_lb = new QLabel(tr("Bottom border height:"), this);
    QLabel *header_active_pix_lb = new QLabel(tr("Header active:"), this);
    QLabel *header_inactive_pix_lb = new QLabel(tr("Header inactive:"), this);
    QLabel *header_color = new QLabel(tr("Title color:"), this);
    QLabel *minmax_pix_lb = new QLabel(tr("Minimize/Maximize:"), this);
    QLabel *close_pix_lb = new QLabel(tr("Close:"), this);
    header_active_pix = new QLabel(this);
    header_inactive_pix = new QLabel(this);
    header_active_pix->setMaximumSize(32, 32);
    header_inactive_pix->setMaximumSize(32, 32);
    header_active_pix->setScaledContents(true);
    header_inactive_pix->setScaledContents(true);
    header_col_lab = new QLabel(this);
    header_col_lab->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    header_col_lab->setMinimumWidth(32);
    minmax_pix = new QLabel(this);
    minmax_pix->setMaximumSize(32, 32);
    minmax_pix->setScaledContents(true);
    close_pix = new QLabel(this);
    close_pix->setMaximumSize(32, 32);
    close_pix->setScaledContents(true);
    top_bdr_spinBox = new QSpinBox(this);
    top_bdr_spinBox->setMaximumWidth(70);
    top_bdr_spinBox->setSuffix(" pixels");
    lateral_bdr_spinBox = new QSpinBox(this);
    lateral_bdr_spinBox->setMaximumWidth(70);
    lateral_bdr_spinBox->setSuffix(" pixels");
    bottom_bdr_spinBox = new QSpinBox(this);
    bottom_bdr_spinBox->setMaximumWidth(70);
    bottom_bdr_spinBox->setSuffix(" pixels");
    QPushButton *header_active_pix_but = new QPushButton("...", this);
    QPushButton *header_inactive_pix_but = new QPushButton("...", this);
    header_active_pix_but->setMaximumWidth(50);
    header_inactive_pix_but->setMaximumWidth(50);
    QPushButton *minmax_pix_but = new QPushButton("...", this);
    minmax_pix_but->setMaximumWidth(50);
    QPushButton *close_pix_but = new QPushButton("...", this);
    close_pix_but->setMaximumWidth(50);
    QPushButton *header_col_but = new QPushButton("...", this);
    header_col_but->setMaximumWidth(50);
    frame_grid->addWidget(top_bdr_lb, 0, 0);
    frame_grid->addWidget(lateral_bdr_lb, 1, 0);
    frame_grid->addWidget(bottom_bdr_lb, 2, 0);
    frame_grid->addWidget(header_active_pix_lb, 3, 0);
    frame_grid->addWidget(header_inactive_pix_lb, 4, 0);
    frame_grid->addWidget(header_color, 5, 0);
    frame_grid->addWidget(minmax_pix_lb, 6, 0);
    frame_grid->addWidget(close_pix_lb, 7, 0);
    frame_grid->addWidget(top_bdr_spinBox, 0, 1);
    frame_grid->addWidget(lateral_bdr_spinBox, 1, 1);
    frame_grid->addWidget(bottom_bdr_spinBox, 2, 1);
    frame_grid->addWidget(header_active_pix, 3, 1, Qt::AlignCenter);
    frame_grid->addWidget(header_inactive_pix, 4, 1, Qt::AlignCenter);
    frame_grid->addWidget(header_col_lab, 5, 1, Qt::AlignCenter);
    frame_grid->addWidget(minmax_pix, 6, 1, Qt::AlignCenter);
    frame_grid->addWidget(close_pix, 7, 1, Qt::AlignCenter);
    frame_grid->addWidget(header_active_pix_but, 3, 2);
    frame_grid->addWidget(header_inactive_pix_but, 4, 2);
    frame_grid->addWidget(header_col_but, 5, 2);
    frame_grid->addWidget(minmax_pix_but, 6, 2);
    frame_grid->addWidget(close_pix_but, 7, 2);
    ///////// DOCKBAR /////////
    dockbar_widget = new QWidget(this);
    dockbar_layout = new QGridLayout();
    dockbar_widget->setLayout(dockbar_layout);
    dockbar_box = new QGroupBox(tr("Dockbar"), this);
    dockbar_grid = new QGridLayout();
    dockbar_grid->setSpacing(2);
    dockbar_box->setLayout(dockbar_grid);
    QLabel *dockbar_height_lb = new QLabel(tr("Height:"), this);
    dockbar_height_spinBox = new QSpinBox(this);
    dockbar_height_spinBox->setMaximumWidth(100);
    dockbar_height_spinBox->setSuffix(" pixels");
    QLabel *dockbar_width_lb = new QLabel(tr("Width:"), this);
    dockbar_width_spinBox = new QSpinBox(this);
    dockbar_width_spinBox->setMaximumWidth(100);
    dockbar_width_spinBox->setSuffix(" pixels");
    dockbar_width_spinBox->setMaximum(QApplication::desktop()->width());
    dockbar_width_spinBox->setMinimum(300);
    QLabel *dockbar_position_lb = new QLabel(tr("Position:"), this);
    dockbar_position_comboBox = new QComboBox(this);
    dockbar_position_comboBox->addItem(tr("Bottom"));
    dockbar_position_comboBox->addItem(tr("Top"));
    QLabel *dockbar_pix_lb = new QLabel(tr("Pixmap:"), this);
    dockbar_pix = new QLabel(this);
    dockbar_pix->setMaximumSize(32, 32);
    dockbar_pix->setScaledContents(true);
    QPushButton *dockbar_pix_but = new QPushButton("...", this);
    dockbar_pix_but->setMaximumWidth(50);
    dockbar_grid->addWidget(dockbar_height_lb, 0, 0);
    dockbar_grid->addWidget(dockbar_height_spinBox, 0, 1);
    dockbar_grid->addWidget(dockbar_width_lb, 1, 0);
    dockbar_grid->addWidget(dockbar_width_spinBox, 1, 1);
    dockbar_grid->addWidget(dockbar_position_lb, 2, 0);
    dockbar_grid->addWidget(dockbar_position_comboBox, 2, 1);
    dockbar_grid->addWidget(dockbar_pix_lb, 3, 0);
    dockbar_grid->addWidget(dockbar_pix, 3, 1, Qt::AlignCenter);
    dockbar_grid->addWidget(dockbar_pix_but, 3, 2);
    ///////// DOCKICON /////////
    dockicon_box = new QGroupBox(tr("Dockbar icon"), this);
    dockicon_grid = new QGridLayout();
    dockicon_grid->setSpacing(2);
    dockicon_box->setLayout(dockicon_grid);
    QLabel *dockicon_pix_lb = new QLabel(tr("Pixmap:"), this);
    dockicon_pix = new QLabel(this);
    dockicon_pix->setMaximumSize(32, 32);
    dockicon_pix->setScaledContents(true);
    QLabel *dockicon_color = new QLabel(tr("Title color:"), this);
    dockicon_col_lab = new QLabel(this);
    dockicon_col_lab->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    dockicon_col_lab->setMinimumWidth(32);
    QPushButton *dockicon_pix_but = new QPushButton("...", this);
    dockicon_pix_but->setMaximumWidth(50);
    QPushButton *dockicon_col_but = new QPushButton("...", this);
    dockicon_col_but->setMaximumWidth(50);
    dockicon_grid->addWidget(dockicon_pix_lb, 0, 0);
    dockicon_grid->addWidget(dockicon_pix, 0, 1, Qt::AlignCenter);
    dockicon_grid->addWidget(dockicon_pix_but, 0, 2);
    dockicon_grid->addWidget(dockicon_color, 1, 0);
    dockicon_grid->addWidget(dockicon_col_lab, 1, 1, Qt::AlignCenter);
    dockicon_grid->addWidget(dockicon_col_but, 1, 2);
    ///////// SYSICON /////////
    sysicon_box = new QGroupBox(tr("System Tray icon"), this);
    sysicon_grid = new QGridLayout();
    sysicon_grid->setSpacing(2);
    sysicon_box->setLayout(sysicon_grid);
    QLabel *sysicon_pix_lb = new QLabel(tr("Pixmap:"), this);
    sysicon_pix = new QLabel(this);
    sysicon_pix->setMaximumSize(32, 32);
    sysicon_pix->setScaledContents(true);
    QPushButton *sysicon_pix_but = new QPushButton("...", this);
    sysicon_pix_but->setMaximumWidth(50);
    sysicon_grid->addWidget(sysicon_pix_lb, 0, 0);
    sysicon_grid->addWidget(sysicon_pix, 0, 1, Qt::AlignCenter);
    sysicon_grid->addWidget(sysicon_pix_but, 0, 2);
    ///////// DATECLOCK /////////
    dateclock_box = new QGroupBox(tr("Clock/Date"), this);
    dateclock_grid = new QGridLayout();
    dateclock_grid->setSpacing(2);
    dateclock_box->setLayout(dateclock_grid);
    QLabel *date_color = new QLabel(tr("Date color:"), this);
    QLabel *clock_color = new QLabel(tr("Clock color:"), this);
    date_col_lab = new QLabel(this);
    clock_col_lab = new QLabel(this);
    date_col_lab->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    clock_col_lab->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    date_col_lab->setMinimumWidth(32);
    clock_col_lab->setMinimumWidth(32);
    QPushButton *date_col_but = new QPushButton("...", this);
    QPushButton *clock_col_but = new QPushButton("...", this);
    date_col_but->setMaximumWidth(50);
    clock_col_but->setMaximumWidth(50);
    dateclock_grid->addWidget(date_color, 1, 0);
    dateclock_grid->addWidget(date_col_lab, 1, 1, Qt::AlignCenter);
    dateclock_grid->addWidget(date_col_but, 1, 2);
    dateclock_grid->addWidget(clock_color, 0, 0);
    dateclock_grid->addWidget(clock_col_lab, 0, 1, Qt::AlignCenter);
    dateclock_grid->addWidget(clock_col_but, 0, 2);
    /////////////////////////////////////////////////
    dockbar_layout->addWidget(dockbar_box, 0, 0);
    dockbar_layout->addWidget(dockicon_box, 0, 1);
    dockbar_layout->addWidget(sysicon_box, 1, 0);
    dockbar_layout->addWidget(dateclock_box, 1, 1);
    ///////// DESKTSET WIDGETS //////////////////////
    deskset_widget = new QWidget(this);
    deskset_layout = new QGridLayout();
    deskset_widget->setLayout(deskset_layout);
    ///////// DESKFOLDER ////////////////////////////
    deskfolder_box = new QGroupBox(tr("Desktop folder"), this);
    deskfolder_grid = new QGridLayout();
    deskfolder_grid->setSpacing(2);
    deskfolder_box->setLayout(deskfolder_grid);
    QLabel *deskfolder_pix_lb = new QLabel(tr("Pixmap:"), this);
    deskfolder_pix = new QLabel(this);
    deskfolder_pix->setMaximumSize(32, 32);
    deskfolder_pix->setScaledContents(true);
    QLabel *deskfolder_color = new QLabel(tr("Name color:"), this);
    deskfolder_col_lab = new QLabel(this);
    deskfolder_col_lab->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    deskfolder_col_lab->setMinimumWidth(32);
    QPushButton *deskfolder_pix_but = new QPushButton("...", this);
    deskfolder_pix_but->setMaximumWidth(50);
    QPushButton *deskfolder_col_but = new QPushButton("...", this);
    deskfolder_col_but->setMaximumWidth(50);
    deskfolder_grid->addWidget(deskfolder_pix_lb, 0, 0);
    deskfolder_grid->addWidget(deskfolder_pix, 0, 1, Qt::AlignCenter);
    deskfolder_grid->addWidget(deskfolder_pix_but, 0, 2);
    deskfolder_grid->addWidget(deskfolder_color, 1, 0);
    deskfolder_grid->addWidget(deskfolder_col_lab, 1, 1, Qt::AlignCenter);
    deskfolder_grid->addWidget(deskfolder_col_but, 1, 2);
    ///////// DESKFILE /////////
    deskfile_box = new QGroupBox(tr("Desktop file"), this);
    deskfile_grid = new QGridLayout();
    deskfile_grid->setSpacing(2);
    deskfile_box->setLayout(deskfile_grid);
    QLabel *deskfile_color = new QLabel(tr("Name color:"), this);
    deskfile_col_lab = new QLabel(this);
    deskfile_col_lab->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    deskfile_col_lab->setMinimumWidth(32);
    QPushButton *deskfile_col_but = new QPushButton("...", this);
    deskfile_col_but->setMaximumWidth(50);
    deskfile_grid->addWidget(deskfile_color, 0, 0);
    deskfile_grid->addWidget(deskfile_col_lab, 0, 1, Qt::AlignCenter);
    deskfile_grid->addWidget(deskfile_col_but, 0, 2);
    ///////// DESKAPP /////////
    deskapp_box = new QGroupBox(tr("Desktop application"), this);
    deskapp_grid = new QGridLayout();
    deskapp_grid->setSpacing(2);
    deskapp_box->setLayout(deskapp_grid);
    QLabel *deskapp_color = new QLabel(tr("Name color:"), this);
    deskapp_col_lab = new QLabel(this);
    deskapp_col_lab->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    deskapp_col_lab->setMinimumWidth(32);
    QPushButton *deskapp_col_but = new QPushButton("...", this);
    deskapp_col_but->setMaximumWidth(50);
    deskapp_grid->addWidget(deskapp_color, 0, 0);
    deskapp_grid->addWidget(deskapp_col_lab, 0, 1, Qt::AlignCenter);
    deskapp_grid->addWidget(deskapp_col_but, 0, 2);
    ///////// DESKDEV /////////
    deskdev_box = new QGroupBox(tr("Desktop device"), this);
    deskdev_grid = new QGridLayout();
    deskdev_grid->setSpacing(2);
    deskdev_box->setLayout(deskdev_grid);
    QLabel *deskdev_disk_pix_lb = new QLabel(tr("Disk:"), this);
    deskdev_disk_pix = new QLabel(this);
    deskdev_disk_pix->setMaximumSize(32, 32);
    deskdev_disk_pix->setScaledContents(true);
    QLabel *deskdev_cdrom_pix_lb = new QLabel(tr("CDRom:"), this);
    deskdev_cdrom_pix = new QLabel(this);
    deskdev_cdrom_pix->setMaximumSize(32, 32);
    deskdev_cdrom_pix->setScaledContents(true);
    QLabel *deskdev_color = new QLabel(tr("Name color:"), this);
    deskdev_col_lab = new QLabel(this);
    deskdev_col_lab->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    deskdev_col_lab->setMinimumWidth(32);
    QPushButton *deskdev_disk_pix_but = new QPushButton("...", this);
    deskdev_disk_pix_but->setMaximumWidth(50);
    QPushButton *deskdev_cdrom_pix_but = new QPushButton("...", this);
    deskdev_cdrom_pix_but->setMaximumWidth(50);
    QPushButton *deskdev_col_but = new QPushButton("...", this);
    deskdev_col_but->setMaximumWidth(50);
    deskdev_grid->addWidget(deskdev_disk_pix_lb, 0, 0);
    deskdev_grid->addWidget(deskdev_disk_pix, 0, 1, Qt::AlignCenter);
    deskdev_grid->addWidget(deskdev_disk_pix_but, 0, 2);
    deskdev_grid->addWidget(deskdev_cdrom_pix_lb, 1, 0);
    deskdev_grid->addWidget(deskdev_cdrom_pix, 1, 1, Qt::AlignCenter);
    deskdev_grid->addWidget(deskdev_cdrom_pix_but, 1, 2);
    deskdev_grid->addWidget(deskdev_color, 2, 0);
    deskdev_grid->addWidget(deskdev_col_lab, 2, 1, Qt::AlignCenter);
    deskdev_grid->addWidget(deskdev_col_but, 2, 2);
    ///////// TRASH ////////////////////////////
    trash_box = new QGroupBox(tr("Trash"), this);
    trash_grid = new QGridLayout();
    trash_grid->setSpacing(2);
    trash_box->setLayout(trash_grid);
    QLabel *trash_pix_lb = new QLabel(tr("Pixmap:"), this);
    trash_pix = new QLabel(this);
    trash_pix->setMaximumSize(32, 32);
    trash_pix->setScaledContents(true);
    QLabel *trash_color = new QLabel(tr("Name color:"), this);
    trash_col_lab = new QLabel(this);
    trash_col_lab->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    trash_col_lab->setMinimumWidth(32);
    QPushButton *trash_pix_but = new QPushButton("...", this);
    trash_pix_but->setMaximumWidth(50);
    QPushButton *trash_col_but = new QPushButton("...", this);
    trash_col_but->setMaximumWidth(50);
    trash_grid->addWidget(trash_pix_lb, 0, 0);
    trash_grid->addWidget(trash_pix, 0, 1, Qt::AlignCenter);
    trash_grid->addWidget(trash_pix_but, 0, 2);
    trash_grid->addWidget(trash_color, 1, 0);
    trash_grid->addWidget(trash_col_lab, 1, 1, Qt::AlignCenter);
    trash_grid->addWidget(trash_col_but, 1, 2);
    /////////////////////////////////////////////////
    deskset_layout->addWidget(deskfolder_box, 0, 0);
    deskset_layout->addWidget(deskfile_box, 1, 0);
    deskset_layout->addWidget(deskapp_box, 2, 0);
    deskset_layout->addWidget(deskdev_box, 0, 1);
    deskset_layout->addWidget(trash_box, 1, 1);
    ///////// DESKTOP /////////
    desktop_box = new QGroupBox(tr("Desktop wallpaper"), this);
    desktop_layout = new QGridLayout();
    desktop_box->setLayout(desktop_layout);
    desktop_pix = new QLabel(this);
    desktop_pix->setMaximumSize(300, 250);
    desktop_pix->setScaledContents(true);
    QPushButton *desktop_pix_but = new QPushButton("...", this);
    desktop_pix_but->setMaximumWidth(100);
    desktop_layout->addWidget(desktop_pix, 0, 0, Qt::AlignCenter);
    desktop_layout->addWidget(desktop_pix_but, 1, 0, Qt::AlignCenter);
    ///////// LAUNCHER /////////
    launcher_box = new QGroupBox(tr("Launcher"), this);
    launcher_grid = new QGridLayout();
    launcher_grid->setSpacing(2);
    launcher_box->setLayout(launcher_grid);
    QLabel *launcher_pix_lb = new QLabel(tr("Launcher:"), this);
    QLabel *quit_pix_lb = new QLabel(tr("Quit:"), this);
    QLabel *shutdown_pix_lb = new QLabel(tr("Shutdown:"), this);
    QLabel *restart_pix_lb = new QLabel(tr("Restart:"), this);
    QLabel *refresh_pix_lb = new QLabel(tr("Refresh:"), this);
    QLabel *run_pix_lb = new QLabel(tr("Run:"), this);
    QLabel *show_pix_lb = new QLabel(tr("Show Desktop:"), this);
    QLabel *manager_pix_lb = new QLabel(tr("Manager:"), this);
    QLabel *settings_pix_lb = new QLabel(tr("Settings:"), this);
    launcher_pix = new QLabel(this);
    quit_pix = new QLabel(this);
    shutdown_pix = new QLabel(this);
    restart_pix = new QLabel(this);
    refresh_pix = new QLabel(this);
    run_pix = new QLabel(this);
    show_pix = new QLabel(this);
    manager_pix = new QLabel(this);
    settings_pix = new QLabel(this);
    launcher_pix->setMaximumSize(32, 32);
    quit_pix->setMaximumSize(32, 32);
    shutdown_pix->setMaximumSize(32, 32);
    restart_pix->setMaximumSize(32, 32);
    refresh_pix->setMaximumSize(32, 32);
    run_pix->setMaximumSize(32, 32);
    show_pix->setMaximumSize(32, 32);
    manager_pix->setMaximumSize(32, 32);
    settings_pix->setMaximumSize(32, 32);
    launcher_pix->setScaledContents(true);
    quit_pix->setScaledContents(true);
    shutdown_pix->setScaledContents(true);
    restart_pix->setScaledContents(true);
    refresh_pix->setScaledContents(true);
    run_pix->setScaledContents(true);
    show_pix->setScaledContents(true);
    manager_pix->setScaledContents(true);
    settings_pix->setScaledContents(true);
    QPushButton *launcher_pix_but = new QPushButton("...", this);
    QPushButton *quit_pix_but = new QPushButton("...", this);
    QPushButton *shutdown_pix_but = new QPushButton("...", this);
    QPushButton *restart_pix_but = new QPushButton("...", this);
    QPushButton *refresh_pix_but = new QPushButton("...", this);
    QPushButton *run_pix_but = new QPushButton("...", this);
    QPushButton *show_pix_but = new QPushButton("...", this);
    QPushButton *manager_pix_but = new QPushButton("...", this);
    QPushButton *settings_pix_but = new QPushButton("...", this);
    launcher_pix_but->setMaximumWidth(50);
    quit_pix_but->setMaximumWidth(50);
    shutdown_pix_but->setMaximumWidth(50);
    restart_pix_but->setMaximumWidth(50);
    refresh_pix_but->setMaximumWidth(50);
    run_pix_but->setMaximumWidth(50);
    show_pix_but->setMaximumWidth(50);
    manager_pix_but->setMaximumWidth(50);
    settings_pix_but->setMaximumWidth(50);
    launcher_grid->addWidget(launcher_pix_lb, 0, 0);
    launcher_grid->addWidget(quit_pix_lb, 1, 0);
    launcher_grid->addWidget(shutdown_pix_lb, 2, 0);
    launcher_grid->addWidget(restart_pix_lb, 3, 0);
    launcher_grid->addWidget(refresh_pix_lb, 4, 0);
    launcher_grid->addWidget(run_pix_lb, 5, 0);
    launcher_grid->addWidget(show_pix_lb, 6, 0);
    launcher_grid->addWidget(manager_pix_lb, 7, 0);
    launcher_grid->addWidget(settings_pix_lb, 8, 0);
    launcher_grid->addWidget(launcher_pix, 0, 1, Qt::AlignCenter);
    launcher_grid->addWidget(quit_pix, 1, 1, Qt::AlignCenter);
    launcher_grid->addWidget(shutdown_pix, 2, 1, Qt::AlignCenter);
    launcher_grid->addWidget(restart_pix, 3, 1, Qt::AlignCenter);
    launcher_grid->addWidget(refresh_pix, 4, 1, Qt::AlignCenter);
    launcher_grid->addWidget(run_pix, 5, 1, Qt::AlignCenter);
    launcher_grid->addWidget(show_pix, 6, 1, Qt::AlignCenter);
    launcher_grid->addWidget(manager_pix, 7, 1, Qt::AlignCenter);
    launcher_grid->addWidget(settings_pix, 8, 1, Qt::AlignCenter);
    launcher_grid->addWidget(launcher_pix_but, 0, 2);
    launcher_grid->addWidget(quit_pix_but, 1, 2);
    launcher_grid->addWidget(shutdown_pix_but, 2, 2);
    launcher_grid->addWidget(restart_pix_but, 3, 2);
    launcher_grid->addWidget(refresh_pix_but, 4, 2);
    launcher_grid->addWidget(run_pix_but, 5, 2);
    launcher_grid->addWidget(show_pix_but, 6, 2);
    launcher_grid->addWidget(manager_pix_but, 7, 2);
    launcher_grid->addWidget(settings_pix_but, 8, 2);
    ///////// CATEGORY /////////
    category_box = new QGroupBox(tr("Categories"), this);
    category_grid = new QGridLayout();
    category_grid->setSpacing(2);
    category_box->setLayout(category_grid);
    QLabel *utility_pix_lb = new QLabel(tr("Utility:"), this);
    QLabel *office_pix_lb = new QLabel(tr("Office:"), this);
    QLabel *network_pix_lb = new QLabel(tr("Network:"), this);
    QLabel *graphics_pix_lb = new QLabel(tr("Graphics:"), this);
    QLabel *development_pix_lb = new QLabel(tr("Development:"), this);
    QLabel *system_pix_lb = new QLabel(tr("System:"), this);
    QLabel *audiovideo_pix_lb = new QLabel(tr("AudioVideo:"), this);
    utility_pix = new QLabel(this);
    office_pix = new QLabel(this);
    network_pix = new QLabel(this);
    graphics_pix = new QLabel(this);
    development_pix = new QLabel(this);
    system_pix = new QLabel(this);
    audiovideo_pix = new QLabel(this);
    manager_pix->setMaximumSize(32, 32);
    utility_pix->setMaximumSize(32, 32);
    office_pix->setMaximumSize(32, 32);
    network_pix->setMaximumSize(32, 32);
    graphics_pix->setMaximumSize(32, 32);
    development_pix->setMaximumSize(32, 32);
    system_pix->setMaximumSize(32, 32);
    audiovideo_pix->setMaximumSize(32, 32);
    utility_pix->setScaledContents(true);
    office_pix->setScaledContents(true);
    network_pix->setScaledContents(true);
    graphics_pix->setScaledContents(true);
    development_pix->setScaledContents(true);
    system_pix->setScaledContents(true);
    audiovideo_pix->setScaledContents(true);
    QPushButton *utility_pix_but = new QPushButton("...", this);
    QPushButton *office_pix_but = new QPushButton("...", this);
    QPushButton *network_pix_but = new QPushButton("...", this);
    QPushButton *graphics_pix_but = new QPushButton("...", this);
    QPushButton *development_pix_but = new QPushButton("...", this);
    QPushButton *system_pix_but = new QPushButton("...", this);
    QPushButton *audiovideo_pix_but = new QPushButton("...", this);
    utility_pix_but->setMaximumWidth(50);
    office_pix_but->setMaximumWidth(50);
    network_pix_but->setMaximumWidth(50);
    graphics_pix_but->setMaximumWidth(50);
    development_pix_but->setMaximumWidth(50);
    system_pix_but->setMaximumWidth(50);
    audiovideo_pix_but->setMaximumWidth(50);
    category_grid->addWidget(utility_pix_lb, 0, 0);
    category_grid->addWidget(office_pix_lb, 1, 0);
    category_grid->addWidget(network_pix_lb, 2, 0);
    category_grid->addWidget(graphics_pix_lb, 3, 0);
    category_grid->addWidget(development_pix_lb, 4, 0);
    category_grid->addWidget(system_pix_lb, 5, 0);
    category_grid->addWidget(audiovideo_pix_lb, 6, 0);
    category_grid->addWidget(utility_pix, 0, 1, Qt::AlignCenter);
    category_grid->addWidget(office_pix, 1, 1, Qt::AlignCenter);
    category_grid->addWidget(network_pix, 2, 1, Qt::AlignCenter);
    category_grid->addWidget(graphics_pix, 3, 1, Qt::AlignCenter);
    category_grid->addWidget(development_pix, 4, 1, Qt::AlignCenter);
    category_grid->addWidget(system_pix, 5, 1, Qt::AlignCenter);
    category_grid->addWidget(audiovideo_pix, 6, 1, Qt::AlignCenter);
    category_grid->addWidget(utility_pix_but, 0, 2);
    category_grid->addWidget(office_pix_but, 1, 2);
    category_grid->addWidget(network_pix_but, 2, 2);
    category_grid->addWidget(graphics_pix_but, 3, 2);
    category_grid->addWidget(development_pix_but, 4, 2);
    category_grid->addWidget(system_pix_but, 5, 2);
    category_grid->addWidget(audiovideo_pix_but, 6, 2);
    ///////// MESSAGE /////////
    message_box = new QGroupBox(tr("Message"), this);
    message_grid = new QGridLayout();
    message_grid->setSpacing(2);
    message_box->setLayout(message_grid);
    QLabel *ok_button_pix_lb = new QLabel(tr("Ok:"), this);
    QLabel *close_button_pix_lb = new QLabel(tr("Close:"), this);
    QLabel *add_button_pix_lb = new QLabel(tr("Add:"), this);
    QLabel *remove_button_pix_lb = new QLabel(tr("Remove:"), this);
    QLabel *restore_button_pix_lb = new QLabel(tr("Restore:"), this);
    QLabel *question_pix_lb = new QLabel(tr("Question:"), this);
    QLabel *information_pix_lb = new QLabel(tr("Information:"), this);
    QLabel *warning_pix_lb = new QLabel(tr("Warning:"), this);
    QLabel *critical_pix_lb = new QLabel(tr("Critical:"), this);
    ok_button_pix = new QLabel(this);
    close_button_pix = new QLabel(this);
    add_button_pix = new QLabel(this);
    remove_button_pix = new QLabel(this);
    restore_button_pix = new QLabel(this);
    question_pix = new QLabel(this);
    information_pix = new QLabel(this);
    warning_pix = new QLabel(this);
    critical_pix = new QLabel(this);
    ok_button_pix->setMaximumSize(32, 32);
    close_button_pix->setMaximumSize(32, 32);
    add_button_pix->setMaximumSize(32, 32);
    remove_button_pix->setMaximumSize(32, 32);
    restore_button_pix->setMaximumSize(32, 32);
    question_pix->setMaximumSize(32, 32);
    information_pix->setMaximumSize(32, 32);
    warning_pix->setMaximumSize(32, 32);
    critical_pix->setMaximumSize(32, 32);
    ok_button_pix->setScaledContents(true);
    close_button_pix->setScaledContents(true);
    add_button_pix->setScaledContents(true);
    remove_button_pix->setScaledContents(true);
    restore_button_pix->setScaledContents(true);
    question_pix->setScaledContents(true);
    information_pix->setScaledContents(true);
    warning_pix->setScaledContents(true);
    critical_pix->setScaledContents(true);
    QPushButton *ok_button_pix_but = new QPushButton("...", this);
    QPushButton *close_button_pix_but = new QPushButton("...", this);
    QPushButton *add_button_pix_but = new QPushButton("...", this);
    QPushButton *remove_button_pix_but = new QPushButton("...", this);
    QPushButton *restore_button_pix_but = new QPushButton("...", this);
    QPushButton *question_pix_but = new QPushButton("...", this);
    QPushButton *information_pix_but = new QPushButton("...", this);
    QPushButton *warning_pix_but = new QPushButton("...", this);
    QPushButton *critical_pix_but = new QPushButton("...", this);
    ok_button_pix_but->setMaximumWidth(50);
    close_button_pix_but->setMaximumWidth(50);
    add_button_pix_but->setMaximumWidth(50);
    remove_button_pix_but->setMaximumWidth(50);
    restore_button_pix_but->setMaximumWidth(50);
    question_pix_but->setMaximumWidth(50);
    information_pix_but->setMaximumWidth(50);
    warning_pix_but->setMaximumWidth(50);
    critical_pix_but->setMaximumWidth(50);
    message_grid ->addWidget(ok_button_pix_lb, 0, 0);
    message_grid ->addWidget(close_button_pix_lb , 1, 0);
    message_grid ->addWidget(add_button_pix_lb , 2, 0);
    message_grid ->addWidget(remove_button_pix_lb , 3, 0);
    message_grid ->addWidget(restore_button_pix_lb , 4, 0);
    message_grid ->addWidget(question_pix_lb, 5, 0);
    message_grid ->addWidget(information_pix_lb, 6, 0);
    message_grid ->addWidget(warning_pix_lb, 7, 0);
    message_grid ->addWidget(critical_pix_lb, 8, 0);
    message_grid ->addWidget(ok_button_pix, 0, 1, Qt::AlignCenter);
    message_grid ->addWidget(close_button_pix, 1, 1, Qt::AlignCenter);
    message_grid ->addWidget(add_button_pix, 2, 1, Qt::AlignCenter);
    message_grid ->addWidget(remove_button_pix, 3, 1, Qt::AlignCenter);
    message_grid ->addWidget(restore_button_pix, 4, 1, Qt::AlignCenter);
    message_grid ->addWidget(question_pix, 5, 1, Qt::AlignCenter);
    message_grid ->addWidget(information_pix, 6, 1, Qt::AlignCenter);
    message_grid ->addWidget(warning_pix, 7, 1, Qt::AlignCenter);
    message_grid ->addWidget(critical_pix, 8, 1, Qt::AlignCenter);
    message_grid ->addWidget(ok_button_pix_but, 0, 2);
    message_grid ->addWidget(close_button_pix_but, 1, 2);
    message_grid ->addWidget(add_button_pix_but, 2, 2);
    message_grid ->addWidget(remove_button_pix_but, 3, 2);
    message_grid ->addWidget(restore_button_pix_but, 4, 2);
    message_grid ->addWidget(question_pix_but, 5, 2);
    message_grid ->addWidget(information_pix_but, 6, 2);
    message_grid ->addWidget(warning_pix_but, 7, 2);
    message_grid ->addWidget(critical_pix_but, 8, 2);
    ///////// OTHER /////////
    other_box = new QGroupBox(tr("Other"), this);
    other_grid = new QGridLayout();
    other_grid->setSpacing(2);
    other_box->setLayout(other_grid);
    QLabel *application_pix_lb = new QLabel(tr("Application:"), this);
    QLabel *folder_link_pix_lb = new QLabel(tr("Folder link:"), this);
    QLabel *file_link_pix_lb = new QLabel(tr("File link:"), this);
    QLabel *app_link_pix_lb = new QLabel(tr("Application link:"), this);
    QLabel *delete_link_pix_lb = new QLabel(tr("Delete link:"), this);
    QLabel *delete_file_pix_lb = new QLabel(tr("Delete file:"), this);
    QLabel *cut_file_pix_lb = new QLabel(tr("Cut file:"), this);
    QLabel *copy_file_pix_lb = new QLabel(tr("Copy file:"), this);
    QLabel *paste_file_pix_lb = new QLabel(tr("Paste file:"), this);
    QLabel *close_dock_pix_lb = new QLabel(tr("Close Dock:"), this);
    QLabel *add_to_sys_pix_lb = new QLabel(tr("Add to SysTray:"), this);
    QLabel *open_with_pix_lb = new QLabel(tr("Open with:"), this);
    QLabel *list_view_pix_lb = new QLabel(tr("List view:"), this);
    QLabel *icon_view_pix_lb = new QLabel(tr("Icon view:"), this);
    QLabel *upper_dir_pix_lb = new QLabel(tr("Upper dir:"), this);
    application_pix = new QLabel(this);
    folder_link_pix = new QLabel(this);
    file_link_pix = new QLabel(this);
    app_link_pix = new QLabel(this);
    delete_link_pix = new QLabel(this);
    delete_file_pix = new QLabel(this);
    cut_file_pix = new QLabel(this);
    copy_file_pix = new QLabel(this);
    paste_file_pix = new QLabel(this);
    close_dock_pix = new QLabel(this);
    add_to_sys_pix = new QLabel(this);
    open_with_pix = new QLabel(this);
    list_view_pix = new QLabel(this);
    icon_view_pix = new QLabel(this);
    upper_dir_pix = new QLabel(this);
    application_pix->setMaximumSize(32, 32);
    folder_link_pix->setMaximumSize(32, 32);
    file_link_pix->setMaximumSize(32, 32);
    app_link_pix->setMaximumSize(32, 32);
    delete_link_pix->setMaximumSize(32, 32);
    delete_file_pix->setMaximumSize(32, 32);
    cut_file_pix->setMaximumSize(32, 32);
    copy_file_pix->setMaximumSize(32, 32);
    paste_file_pix->setMaximumSize(32, 32);
    close_dock_pix->setMaximumSize(32, 32);
    add_to_sys_pix->setMaximumSize(32, 32);
    open_with_pix->setMaximumSize(32, 32);
    list_view_pix->setMaximumSize(32, 32);
    icon_view_pix->setMaximumSize(32, 32);
    upper_dir_pix->setMaximumSize(32, 32);
    application_pix->setScaledContents(true);
    folder_link_pix->setScaledContents(true);
    file_link_pix->setScaledContents(true);
    app_link_pix->setScaledContents(true);
    delete_link_pix->setScaledContents(true);
    delete_file_pix->setScaledContents(true);
    cut_file_pix->setScaledContents(true);
    copy_file_pix->setScaledContents(true);
    paste_file_pix->setScaledContents(true);
    close_dock_pix->setScaledContents(true);
    add_to_sys_pix->setScaledContents(true);
    open_with_pix->setScaledContents(true);
    list_view_pix->setScaledContents(true);
    icon_view_pix->setScaledContents(true);
    upper_dir_pix->setScaledContents(true);
    QPushButton *application_pix_but = new QPushButton("...", this);
    QPushButton *folder_link_pix_but = new QPushButton("...", this);
    QPushButton *file_link_pix_but = new QPushButton("...", this);
    QPushButton *app_link_pix_but = new QPushButton("...", this);
    QPushButton *delete_link_pix_but = new QPushButton("...", this);
    QPushButton *delete_file_pix_but = new QPushButton("...", this);
    QPushButton *cut_file_pix_but = new QPushButton("...", this);
    QPushButton *copy_file_pix_but = new QPushButton("...", this);
    QPushButton *paste_file_pix_but = new QPushButton("...", this);
    QPushButton *close_dock_pix_but = new QPushButton("...", this);
    QPushButton *add_to_sys_pix_but = new QPushButton("...", this);
    QPushButton *open_with_pix_but = new QPushButton("...", this);
    QPushButton *list_view_pix_but = new QPushButton("...", this);
    QPushButton *icon_view_pix_but = new QPushButton("...", this);
    QPushButton *upper_dir_pix_but = new QPushButton("...", this);
    application_pix_but->setMaximumWidth(50);
    folder_link_pix_but->setMaximumWidth(50);
    file_link_pix_but->setMaximumWidth(50);
    app_link_pix_but->setMaximumWidth(50);
    delete_link_pix_but->setMaximumWidth(50);
    delete_file_pix_but->setMaximumWidth(50);
    cut_file_pix_but->setMaximumWidth(50);
    copy_file_pix_but->setMaximumWidth(50);
    paste_file_pix_but->setMaximumWidth(50);
    close_dock_pix_but->setMaximumWidth(50);
    add_to_sys_pix_but->setMaximumWidth(50);
    open_with_pix_but->setMaximumWidth(50);
    list_view_pix_but->setMaximumWidth(50);
    icon_view_pix_but->setMaximumWidth(50);
    upper_dir_pix_but->setMaximumWidth(50);
    other_grid->addWidget(application_pix_lb, 0, 0);
    other_grid->addWidget(folder_link_pix_lb, 1, 0);
    other_grid->addWidget(file_link_pix_lb, 2, 0);
    other_grid->addWidget(app_link_pix_lb, 3, 0);
    other_grid->addWidget(delete_link_pix_lb, 4, 0);
    other_grid->addWidget(delete_file_pix_lb, 5, 0);
    other_grid->addWidget(cut_file_pix_lb, 6, 0);
    other_grid->addWidget(copy_file_pix_lb, 0, 3);
    other_grid->addWidget(paste_file_pix_lb, 1, 3);
    other_grid->addWidget(close_dock_pix_lb, 2, 3);
    other_grid->addWidget(add_to_sys_pix_lb, 3, 3);
    other_grid->addWidget(open_with_pix_lb, 4, 3);
    other_grid->addWidget(list_view_pix_lb, 5, 3);
    other_grid->addWidget(icon_view_pix_lb, 6, 3);
    other_grid->addWidget(upper_dir_pix_lb, 7, 3);
    other_grid->addWidget(application_pix, 0, 1, Qt::AlignCenter);
    other_grid->addWidget(folder_link_pix, 1, 1, Qt::AlignCenter);
    other_grid->addWidget(file_link_pix, 2, 1, Qt::AlignCenter);
    other_grid->addWidget(app_link_pix, 3, 1, Qt::AlignCenter);
    other_grid->addWidget(delete_link_pix, 4, 1, Qt::AlignCenter);
    other_grid->addWidget(delete_file_pix, 5, 1, Qt::AlignCenter);
    other_grid->addWidget(cut_file_pix, 6, 1, Qt::AlignCenter);
    other_grid->addWidget(copy_file_pix, 0, 4, Qt::AlignCenter);
    other_grid->addWidget(paste_file_pix, 1, 4, Qt::AlignCenter);
    other_grid->addWidget(close_dock_pix, 2, 4, Qt::AlignCenter);
    other_grid->addWidget(add_to_sys_pix, 3, 4, Qt::AlignCenter);
    other_grid->addWidget(open_with_pix, 4, 4, Qt::AlignCenter);
    other_grid->addWidget(list_view_pix, 5, 4, Qt::AlignCenter);
    other_grid->addWidget(icon_view_pix, 6, 4, Qt::AlignCenter);
    other_grid->addWidget(upper_dir_pix, 7, 4, Qt::AlignCenter);
    other_grid->addWidget(application_pix_but, 0, 2);
    other_grid->addWidget(folder_link_pix_but, 1, 2);
    other_grid->addWidget(file_link_pix_but, 2, 2);
    other_grid->addWidget(app_link_pix_but, 3, 2);
    other_grid->addWidget(delete_link_pix_but, 4, 2);
    other_grid->addWidget(delete_file_pix_but, 5, 2);
    other_grid->addWidget(cut_file_pix_but, 6, 2);
    other_grid->addWidget(copy_file_pix_but, 0, 5);
    other_grid->addWidget(paste_file_pix_but, 1, 5);
    other_grid->addWidget(close_dock_pix_but, 2, 5);
    other_grid->addWidget(add_to_sys_pix_but, 3, 5);
    other_grid->addWidget(open_with_pix_but, 4, 5);
    other_grid->addWidget(list_view_pix_but, 5, 5);
    other_grid->addWidget(icon_view_pix_but, 6, 5);
    other_grid->addWidget(upper_dir_pix_but, 7, 5);
    ///////// OK-QUIT /////////
    QGroupBox *ok_close_box = new QGroupBox(this);
    style_layout->addWidget(ok_close_box);
    QHBoxLayout *ok_close_layout = new QHBoxLayout();
    ok_close_box->setLayout(ok_close_layout);
    style->beginGroup("Message");
    QPushButton *ok_but = new QPushButton(QIcon(stl_path + style->value("ok_button_pix").toString()), tr("Ok"), this);
    QPushButton* close_but = new QPushButton(QIcon(stl_path + style->value("close_button_pix").toString()), tr("Close"), this);
    ok_but->setDefault(true); // default button for this Dialog
    style->endGroup(); // Message
    ok_close_layout->addStretch(1);
    ok_close_layout->addWidget(ok_but);
    ok_close_layout->addWidget(close_but);
    /////////////////////////////////////////////////////////
    params_widget->addWidget(frame_box);
    params_widget->addWidget(dockbar_widget);
    params_widget->addWidget(deskset_widget);
    params_widget->addWidget(desktop_box);
    params_widget->addWidget(launcher_box);
    params_widget->addWidget(category_box);
    params_widget->addWidget(message_box);
    params_widget->addWidget(other_box);
    /////////////////////////////////////////////////////////
    connect(sections_widget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this, SLOT(changeSection(QListWidgetItem *, QListWidgetItem *)));
    connect(ok_but, SIGNAL(clicked()), this, SLOT(ok_frame_pressed()));
    connect(close_but, SIGNAL(clicked()), this, SLOT(close_pressed()));

    // mapper for pixmap selection (dockbar - dockicon - sysicon - deskfolder - deskdev - trash - desktop - frame header active/inactive - frame button - all laucher button - message - other)
    pixmapMapper = new QSignalMapper();
    pixmapMapper->setMapping(dockbar_pix_but, dockbar_pix);
    pixmapMapper->setMapping(desktop_pix_but, desktop_pix);
    pixmapMapper->setMapping(dockicon_pix_but, dockicon_pix);
    pixmapMapper->setMapping(sysicon_pix_but, sysicon_pix);
    pixmapMapper->setMapping(deskfolder_pix_but, deskfolder_pix);
    pixmapMapper->setMapping(deskdev_disk_pix_but, deskdev_disk_pix);
    pixmapMapper->setMapping(deskdev_cdrom_pix_but, deskdev_cdrom_pix);
    pixmapMapper->setMapping(trash_pix_but, trash_pix);
    pixmapMapper->setMapping(header_active_pix_but, header_active_pix);
    pixmapMapper->setMapping(header_inactive_pix_but, header_inactive_pix);
    pixmapMapper->setMapping(minmax_pix_but, minmax_pix);
    pixmapMapper->setMapping(close_pix_but, close_pix);
    ///////// Launcher //////////
    pixmapMapper->setMapping(launcher_pix_but, launcher_pix);
    pixmapMapper->setMapping(quit_pix_but, quit_pix);
    pixmapMapper->setMapping(shutdown_pix_but, shutdown_pix);
    pixmapMapper->setMapping(restart_pix_but, restart_pix);
    pixmapMapper->setMapping(refresh_pix_but, refresh_pix);
    pixmapMapper->setMapping(run_pix_but, run_pix);
    pixmapMapper->setMapping(show_pix_but, show_pix);
    pixmapMapper->setMapping(manager_pix_but, manager_pix);
    pixmapMapper->setMapping(settings_pix_but, settings_pix);
    pixmapMapper->setMapping(utility_pix_but, utility_pix);
    pixmapMapper->setMapping(office_pix_but, office_pix);
    pixmapMapper->setMapping(network_pix_but, network_pix);
    pixmapMapper->setMapping(graphics_pix_but, graphics_pix);
    pixmapMapper->setMapping(development_pix_but, development_pix);
    pixmapMapper->setMapping(system_pix_but, system_pix);
    pixmapMapper->setMapping(audiovideo_pix_but, audiovideo_pix);
    ///////// Message //////////
    pixmapMapper->setMapping(ok_button_pix_but, ok_button_pix);
    pixmapMapper->setMapping(close_button_pix_but, close_button_pix);
    pixmapMapper->setMapping(add_button_pix_but, add_button_pix);
    pixmapMapper->setMapping(remove_button_pix_but, remove_button_pix);
    pixmapMapper->setMapping(restore_button_pix_but, restore_button_pix);
    pixmapMapper->setMapping(question_pix_but, question_pix);
    pixmapMapper->setMapping(information_pix_but, information_pix);
    pixmapMapper->setMapping(warning_pix_but, warning_pix);
    pixmapMapper->setMapping(critical_pix_but, critical_pix);
    ///////// Other //////////
    pixmapMapper->setMapping(application_pix_but, application_pix);
    pixmapMapper->setMapping(folder_link_pix_but, folder_link_pix);
    pixmapMapper->setMapping(file_link_pix_but, file_link_pix);
    pixmapMapper->setMapping(app_link_pix_but, app_link_pix);
    pixmapMapper->setMapping(delete_link_pix_but, delete_link_pix);
    pixmapMapper->setMapping(delete_file_pix_but, delete_file_pix);
    pixmapMapper->setMapping(cut_file_pix_but, cut_file_pix);
    pixmapMapper->setMapping(copy_file_pix_but, copy_file_pix);
    pixmapMapper->setMapping(paste_file_pix_but, paste_file_pix);
    pixmapMapper->setMapping(close_dock_pix_but, close_dock_pix);
    pixmapMapper->setMapping(add_to_sys_pix_but, add_to_sys_pix);
    pixmapMapper->setMapping(open_with_pix_but, open_with_pix);
    pixmapMapper->setMapping(list_view_pix_but, list_view_pix);
    pixmapMapper->setMapping(icon_view_pix_but, icon_view_pix);
    pixmapMapper->setMapping(upper_dir_pix_but, upper_dir_pix);
    //////////////////////////////////////////////////////////////////////
    connect(dockbar_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(desktop_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(dockicon_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(sysicon_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(deskfolder_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(deskdev_disk_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(deskdev_cdrom_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(trash_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(header_active_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(header_inactive_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(minmax_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(close_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    ///////// Launcher //////////
    connect(launcher_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(quit_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(shutdown_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(restart_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(refresh_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(run_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(show_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(manager_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(settings_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(utility_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(office_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(network_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(graphics_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(development_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(system_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(audiovideo_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    ///////// Message //////////
    connect(ok_button_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(close_button_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(add_button_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(remove_button_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(restore_button_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(question_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(information_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(warning_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(critical_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    ///////// Other //////////
    connect(application_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(folder_link_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(file_link_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(app_link_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(delete_link_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(delete_file_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(cut_file_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(copy_file_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(paste_file_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(close_dock_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(add_to_sys_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(open_with_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(list_view_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(icon_view_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));
    connect(upper_dir_pix_but, SIGNAL(clicked()), pixmapMapper, SLOT(map()));

    connect(pixmapMapper, SIGNAL(mapped(QWidget *)), this, SLOT(select_pixmap(QWidget *)));

    // mapper for color selection (frame header text - dockicon text - deskfile name - deskfolder name - deskapp name - deskdev name - trash name - date/clock)
    colorMapper = new QSignalMapper();
    colorMapper->setMapping(header_col_but, header_col_lab);
    colorMapper->setMapping(dockicon_col_but, dockicon_col_lab);
    colorMapper->setMapping(deskfolder_col_but, deskfolder_col_lab);
    colorMapper->setMapping(deskfile_col_but, deskfile_col_lab);
    colorMapper->setMapping(deskdev_col_but, deskdev_col_lab);
    colorMapper->setMapping(trash_col_but, trash_col_lab);
    colorMapper->setMapping(deskapp_col_but, deskapp_col_lab);
    colorMapper->setMapping(date_col_but, date_col_lab);
    colorMapper->setMapping(clock_col_but, clock_col_lab);
    connect(header_col_but, SIGNAL(clicked()), colorMapper, SLOT(map()));
    connect(dockicon_col_but, SIGNAL(clicked()), colorMapper, SLOT (map()));
    connect(deskfolder_col_but, SIGNAL(clicked()), colorMapper, SLOT (map()));
    connect(deskfile_col_but, SIGNAL(clicked()), colorMapper, SLOT (map()));
    connect(deskdev_col_but, SIGNAL(clicked()), colorMapper, SLOT (map()));
    connect(trash_col_but, SIGNAL(clicked()), colorMapper, SLOT (map()));
    connect(deskapp_col_but, SIGNAL(clicked()), colorMapper, SLOT (map()));
    connect(date_col_but, SIGNAL(clicked()), colorMapper, SLOT (map()));
    connect(clock_col_but, SIGNAL(clicked()), colorMapper, SLOT (map()));
    connect(colorMapper, SIGNAL(mapped(QWidget *)), this, SLOT(select_color(QWidget *)));
}

void Manager::changeSection(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    params_widget->setCurrentIndex(sections_widget->row(current));
    qDebug() << "Current:" << current;
}

void Manager::change_path(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    tree_view->setRootIndex(fs_model->index(current->text()));
    tree_view->collapseAll();
    app_path->setText(current->text());
}

void Manager::set_run_args(QTableWidgetItem *item) // to set run arguments for startup applications
{
    QTableWidgetItem *name_item = run_table->item(item->row(), 0); // get the app name
    antico->beginGroup("Startup");
    antico->beginGroup(name_item->text()); // application name
    antico->setValue("args", item->text()); // application args on startup
    antico->endGroup(); // Name
    antico->endGroup(); // Startup
}

void Manager::path_completer() // on user button press in line_path
{
    QModelIndex index = fs_model->index(app_path->text());
    tree_view->selectionModel()->clearSelection();
    tree_view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
}

void Manager::select_style()
{
    file_dialog->set_type(tr("Select the style"), "OK_Close");
    file_dialog->raise();
    QStringList filters;
    filters << "*.stl";
    file_dialog->set_name_filters(filters); // show only style file (.stl)
    file_dialog->set_filter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);
    file_dialog->set_path(QCoreApplication::applicationDirPath() + "/theme/");
    file_dialog->set_read_only(true);

    if (file_dialog->exec() == QDialog::Accepted)
    {
        QString path = file_dialog->get_selected_path();
        QString name = file_dialog->get_selected_name();
        stl_name = name; // update path and name style
        stl_path = path;
        qDebug() << "Style path:" << stl_path;
        qDebug() << "Style name:" << stl_name;
        style_sel_tx->setText(name); // update style name on box
        style = new QSettings(stl_path + stl_name, QSettings::IniFormat);
        read_settings();
    }
}

void Manager::select_pixmap(QWidget *pix)
{
    pixmap = (QLabel *)pix;
    file_dialog->set_type(tr("Select the pixmap"), "OK_Close");
    file_dialog->set_path(stl_path);
    file_dialog->set_read_only(true);
    file_dialog->raise();

    if (file_dialog->exec() == QDialog::Accepted)
    {
        QString pix_path = file_dialog->get_selected_path();
        QString pix_name = file_dialog->get_selected_name();
        pixmap->setPixmap(QPixmap(pix_path + pix_name)); // set pixmap
        pixmap->setToolTip(pix_name); // set tooltip (see ok_frame_pressed)
    }
}

void Manager::select_color(QWidget *col)
{
    color_lab = (QLabel *)col;
    Colordialog *diag = new Colordialog(color_lab->palette().color(QPalette::Window)); // update colordialog with actual color

    if (diag->exec() == QDialog::Accepted)
    {
        QColor col = diag->get_color();
        color_lab->setPalette(QPalette(col));
        color_lab->setAutoFillBackground(true);
    }
}

void Manager::ok_frame_pressed()
{
    //////////////////////////////////////////////////////////////////
    antico->beginGroup("Style"); // save the style
    antico->setValue("name", stl_name);
    antico->setValue("path", stl_path);
    antico->endGroup(); //Style
    //////////////////////////////////////////////////////////////////
    style->beginGroup("Frame");
    style->beginGroup("Border");
    style->setValue("top_bdr_height", top_bdr_spinBox->value());
    style->setValue("lateral_bdr_width", lateral_bdr_spinBox->value());
    style->setValue("bottom_bdr_height", bottom_bdr_spinBox->value());
    style->endGroup(); //Border
    //////////////////////////////////////////////////////////////////
    style->beginGroup("Header");
    style->setValue("header_active_pix", header_active_pix->toolTip());
    style->setValue("header_inactive_pix", header_inactive_pix->toolTip());
    style->setValue("title_color", header_col_lab->palette().color(QPalette::Window));
    style->setValue("minmax_pix", minmax_pix->toolTip());
    style->setValue("close_pix", close_pix->toolTip());
    style->endGroup(); //Header
    style->endGroup(); //Frame
    //////////////////////////////////////////////////////////////////
    style->beginGroup("Dockbar");
    style->setValue("dock_pix", dockbar_pix->toolTip());
    style->setValue("dock_height", dockbar_height_spinBox->value());
    style->setValue("dock_width", dockbar_width_spinBox->value());
    style->setValue("dock_position", dockbar_position_comboBox->currentIndex());
    style->endGroup(); //Dockbar
    //////////////////////////////////////////////////////////////////
    style->beginGroup("Dockicon");
    style->setValue("d_icon_pix", dockicon_pix->toolTip());
    style->setValue("title_color", dockicon_col_lab->palette().color(QPalette::Window));
    style->endGroup(); //Dockicon
    //////////////////////////////////////////////////////////////////
    style->beginGroup("Sysicon");
    style->setValue("s_icon_pix", sysicon_pix->toolTip());
    style->endGroup(); //Sysicon
    //////////////////////////////////////////////////////////////////
    style->beginGroup("Deskapp");
    style->setValue("name_color", deskapp_col_lab->palette().color(QPalette::Window));
    style->endGroup(); //Deskapp
    //////////////////////////////////////////////////////////////////
    style->beginGroup("Desktop");
    style->setValue("wall_pix", desktop_pix->toolTip());
    style->endGroup(); //Desktop
    //////////////////////////////////////////////////////////////////
    style->beginGroup("Deskfolder");
    style->setValue("d_folder_pix", deskfolder_pix->toolTip());
    style->setValue("name_color", deskfolder_col_lab->palette().color(QPalette::Window));
    style->endGroup(); //Deskfolder
    //////////////////////////////////////////////////////////////////
    style->beginGroup("Deskfile");
    style->setValue("name_color", deskfile_col_lab->palette().color(QPalette::Window));
    style->endGroup(); //Deskfile
    //////////////////////////////////////////////////////////////////
    style->beginGroup("Deskdev");
    style->setValue("d_disk_pix", deskdev_disk_pix->toolTip());
    style->setValue("d_cdrom_pix", deskdev_cdrom_pix->toolTip());
    style->setValue("name_color", deskdev_col_lab->palette().color(QPalette::Window));
    style->endGroup(); //Deskdev
    //////////////////////////////////////////////////////////////////
    style->beginGroup("Trash");
    style->setValue("trash_pix", trash_pix->toolTip());
    style->setValue("name_color", trash_col_lab->palette().color(QPalette::Window));
    style->endGroup(); //Trash
    //////////////////////////////////////////////////////////////////
    style->beginGroup("Dateclock");
    style->setValue("date_color", date_col_lab->palette().color(QPalette::Window));
    style->setValue("clock_color", clock_col_lab->palette().color(QPalette::Window));
    style->endGroup(); //Dateclock
    //////////////////////////////////////////////////////////////////
    style->beginGroup("Launcher");
    style->setValue("launcher_pix", launcher_pix->toolTip());
    style->setValue("quit_pix", quit_pix->toolTip());
    style->setValue("shutdown_pix", shutdown_pix->toolTip());
    style->setValue("restart_pix", restart_pix->toolTip());
    style->setValue("refresh_pix", refresh_pix->toolTip());
    style->setValue("run_pix", run_pix->toolTip());
    style->setValue("show_pix", show_pix->toolTip());
    style->setValue("manager_pix", manager_pix->toolTip());
    style->setValue("settings_pix", settings_pix->toolTip());
    style->setValue("utility_pix", utility_pix->toolTip());
    style->setValue("office_pix", office_pix->toolTip());
    style->setValue("network_pix", network_pix->toolTip());
    style->setValue("graphics_pix", graphics_pix->toolTip());
    style->setValue("development_pix", development_pix->toolTip());
    style->setValue("system_pix", system_pix->toolTip());
    style->setValue("audiovideo_pix", audiovideo_pix->toolTip());
    style->endGroup(); //Launcher
    //////////////////////////////////////////////////////////////////
    style->beginGroup("Message");
    style->setValue("ok_button_pix", ok_button_pix->toolTip());
    style->setValue("close_button_pix", close_button_pix->toolTip());
    style->setValue("add_button_pix", add_button_pix->toolTip());
    style->setValue("remove_button_pix", remove_button_pix->toolTip());
    style->setValue("restore_button_pix", restore_button_pix->toolTip());
    style->setValue("question_pix", question_pix->toolTip());
    style->setValue("information_pix", information_pix->toolTip());
    style->setValue("warning_pix", warning_pix->toolTip());
    style->setValue("critical_pix", critical_pix->toolTip());
    style->endGroup(); //Message
    //////////////////////////////////////////////////////////////////
    style->beginGroup("Other");
    style->setValue("application_pix", application_pix->toolTip());
    style->setValue("folder_link_pix", folder_link_pix->toolTip());
    style->setValue("file_link_pix", file_link_pix->toolTip());
    style->setValue("app_link_pix", app_link_pix->toolTip());
    style->setValue("delete_link_pix", delete_link_pix->toolTip());
    style->setValue("delete_file_pix", delete_file_pix->toolTip());
    style->setValue("cut_file_pix", cut_file_pix->toolTip());
    style->setValue("copy_file_pix", copy_file_pix->toolTip());
    style->setValue("paste_file_pix", paste_file_pix->toolTip());
    style->setValue("close_dock_pix", close_dock_pix->toolTip());
    style->setValue("add_to_sys_pix", add_to_sys_pix->toolTip());
    style->setValue("open_with_pix", open_with_pix->toolTip());
    style->setValue("list_view_pix", list_view_pix->toolTip());
    style->setValue("icon_view_pix", icon_view_pix->toolTip());
    style->setValue("upper_dir_pix", upper_dir_pix->toolTip());
    style->endGroup(); //Other
    //////////////////////////////////////////////////////////////////

    Msgbox msg;
    msg.set_header(tr("ANTICO STYLE SETTINGS UPDATED"));
    msg.set_info(tr("To apply the modify, select <b>Refresh WM</b> on Launcher menu"));
    msg.set_icon("Information");
    msg.exec();
}

void Manager::add_app_pressed() // add selected app on lancher menu (in the select Category)
{
    if (tree_view->selectionModel()->currentIndex().isValid())
    {
        QString path = fs_model->filePath(tree_view->selectionModel()->currentIndex());
        QString app = fs_model->fileName(tree_view->selectionModel()->currentIndex());
        QFileInfo info = fs_model->fileInfo(tree_view->selectionModel()->currentIndex());
        Appicon app_ico; // get application icon
        QString icon = app_ico.get_app_icon(app);

        if ( ! app.isEmpty() && info.isExecutable() && ! info.isDir())
        {
            qDebug() << "app:" << app << "path:" << path;
            antico->beginGroup("Launcher");
            antico->beginGroup("Category");
            antico->beginGroup(cat_map.value(category_combo->currentText())); // the select Category type from combobox (with translation)
            antico->beginGroup(app);

            antico->setValue("name", app);
            antico->setValue("exec", path);
            antico->setValue("pix", icon);
	    qDebug() << app << " " << path << " " << icon ;
            antico->endGroup(); // App name
            antico->endGroup(); // Category type
            antico->endGroup(); // Category group
            antico->endGroup(); // Launcher
            update_remove_list();
            Msgbox msg;
            msg.set_header(tr("APPLICATION ADDED"));
            msg.set_info(tr("To apply the modify, select <b>Refresh WM</b> on Launcher menu"));
            msg.set_icon("Information");
            msg.exec();
        }
    }
}

void Manager::add_run_app_pressed() // add selected app on "Run at startup" list
{
    file_dialog->set_type(tr("Add application to run at startup:"), "OK_Close");
    file_dialog->raise();

    if (file_dialog->exec() == QDialog::Accepted)
    {
        QString path = file_dialog->get_selected_path();
        QString name = file_dialog->get_selected_name();
        QFileInfo pathinfo(path+name);

        if (! name.isEmpty() && pathinfo.isExecutable())
        {
            qDebug() << "App:" << name << "Path:" << path;
            Appicon app_icon; // get application icon
            QString icon = app_icon.get_app_icon(name);
            QTableWidgetItem *name_item = new QTableWidgetItem(QIcon(icon), name); // add app name on run table
            QTableWidgetItem *path_item = new QTableWidgetItem(path); // add app path on run table
            QTableWidgetItem *args_item = new QTableWidgetItem(""); // add app args on run table
            name_item->setFlags(Qt::ItemIsEnabled);
            path_item->setFlags(Qt::ItemIsEnabled);
            run_table->insertRow(run_table->rowCount());
            run_table->setItem(run_table->rowCount()-1, 0, name_item);
            run_table->setItem(run_table->rowCount()-1, 1, path_item);
            run_table->setItem(run_table->rowCount()-1, 2, args_item);
            run_table->resizeColumnsToContents();
            antico->beginGroup("Startup");
            antico->beginGroup(name);
            antico->setValue("name", name);
            antico->setValue("path", path);
            antico->setValue("args", ""); // for future settings
            antico->endGroup(); // Name
            antico->endGroup(); // Startup
        }
    }
}

void Manager::remove_run_app_pressed() // remove selected app from "Run at startup" list
{
    int current_row = run_table->currentRow();

    if (current_row >= 0) // if some row is selected
    {
        QTableWidgetItem *name_item = run_table->item(current_row, 0); // get the app name
        antico->beginGroup("Startup");
        antico->remove(name_item->text()); // remove the application
        antico->endGroup(); // Startup
        run_table->removeRow(current_row); // remove the application from run list
    }
}

void Manager::remove_app_pressed() // remove selected app from Launcher menu
{
    if (app_tree->currentItem() != NULL && app_tree->currentItem()->parent() != NULL) // remove only is item is selected (no if it's a Category)
    {
        QString app = app_tree->currentItem()->text(0);
        QString cat_group = app_tree->currentItem()->parent()->text(0);
        qDebug() << "Application:" << app;
        antico->beginGroup("Launcher");
        antico->beginGroup("Category");
        antico->beginGroup(cat_group); // Category group (parent of application)
        qDebug() << "Parent Application:" << cat_group;
        antico->remove(app); // remove the application
        antico->endGroup(); // Category group
        antico->endGroup(); // Category
        antico->endGroup(); // Launcher
        update_remove_list();
        Msgbox msg;
        msg.set_header(tr("APPLICATION REMOVED"));
        msg.set_info(tr("To apply the modify, select <b>Refresh WM</b> on Launcher menu"));
        msg.set_icon("Information");
        msg.exec();
    }
}

void Manager::update_remove_list() // update the Category/Apps list on remove tab
{
    app_tree->clear();
    antico->beginGroup("Launcher");
    antico->beginGroup("Category");

    for (int c = 0; c < antico->childGroups().size(); c++)
    {
        QString cat_name = antico->childGroups().value(c); //Category group (Utility, Office ...)
        antico->beginGroup(cat_name); // Category group
        category = new QTreeWidgetItem(app_tree); // add Category group on TreeWidget
        category->setText(0, cat_name); // set Category group name (top-level item)
        category->setBackground(0, QBrush(Qt::gray));

        for (int i = 0; i < antico->childGroups().size(); i++)
        {
            QString app_name = antico->childGroups().value(i); //Application name
            app = new QTreeWidgetItem(category); // add Application on Category tree
            Appicon app_icon; // get application icon
            QString icon = app_icon.get_app_icon(app_name);
            app->setText(0, app_name);
            app->setIcon(0, QIcon(icon));
        }
        antico->endGroup(); // Category group
    }
    antico->endGroup();// Category
    antico->endGroup();// Launcher
}

void Manager::update_run_table() // update the "Run at startup" list
{
    run_table->clearContents();

    antico->beginGroup("Startup");

    for (int i = 0; i < antico->childGroups().size(); i++)
    {
        QString app_name = antico->childGroups().value(i);
        antico->beginGroup(app_name); // App group
        QString path = antico->value("path").toString(); // App path
        QString args = antico->value("args").toString(); // App arguments
        Appicon app_icon; // get application icon
        QString icon = app_icon.get_app_icon(app_name);
        QTableWidgetItem *name_item = new QTableWidgetItem(QIcon(icon), app_name); // add app on run table
        QTableWidgetItem *path_item = new QTableWidgetItem(path); // add app path on run table
        QTableWidgetItem *args_item = new QTableWidgetItem(args); // add app args on run table
        name_item->setFlags(Qt::ItemIsEnabled);
        path_item->setFlags(Qt::ItemIsEnabled);
        run_table->insertRow(run_table->rowCount());
        run_table->setItem(run_table->rowCount()-1, 0, name_item);
        run_table->setItem(run_table->rowCount()-1, 1, path_item);
        run_table->setItem(run_table->rowCount()-1, 2, args_item);
        antico->endGroup(); // Name
    }
    antico->endGroup(); // Startup
}

void Manager::close_pressed()
{
    close();
}

void Manager::show_path(const QModelIndex &index)
{
    app_path->setText(fs_model->filePath(index));
}

void Manager::mousePressEvent(QMouseEvent *event)
{
    mousepos = event->pos();
    grabMouse(QCursor(Qt::SizeAllCursor));
}

void Manager::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p = event->globalPos()-mousepos;
    move(p.x(), p.y());
}

void Manager::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    releaseMouse();
}



