////////////////////////////////////////
//  File      : filedialog.cpp        //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "filedialog.h"

////////////////////////////////////////

Filedialog::Filedialog(Categorymenu *menu, QWidget *parent) : QDialog(parent)
{
    cat_menu = menu;
    setSizeGripEnabled(true);
    setWindowModality(Qt::WindowModal);
    setAttribute(Qt::WA_AlwaysShowToolTips);
    read_settings();
    init();
    set_category_menu();
    set_list_mode(); // list view at startup
}

Filedialog::Filedialog(QWidget *parent) : QDialog(parent) // without Category menu
{
    cat_menu = NULL;
    setSizeGripEnabled(true);
    setWindowModality(Qt::WindowModal);
    setAttribute(Qt::WA_AlwaysShowToolTips);
    read_settings();
    init();
    set_list_mode(); // list view at startup
}

Filedialog::~Filedialog()
{
    delete ok;
    delete cancel;
    delete close;
    delete antico;
    delete root_item;
    delete bin_item;
    delete home_item;
    delete cut_act;
    delete copy_act;
    delete paste_act;
    delete abstract_view;
    delete open_menu;
    delete cat_menu;
    delete prov;
}

void Filedialog::read_settings()
{
    // get style path
    antico = new QSettings(QDir::homePath() + "/.razor/razor.cfg", QSettings::IniFormat, this);
    antico->beginGroup("Style");
    QString stl_name = antico->value("name").toString();
    QString stl_path = antico->value("path").toString();
    antico->endGroup(); //Style
    // get style values
    QSettings *style = new QSettings(stl_path + stl_name, QSettings::IniFormat, this);
    style->beginGroup("Other");
    delete_file_pix = stl_path + style->value("delete_file_pix").toString();
    cut_file_pix = stl_path + style->value("cut_file_pix").toString();
    copy_file_pix = stl_path + style->value("copy_file_pix").toString();
    paste_file_pix = stl_path + style->value("paste_file_pix").toString();
    edit_file_pix = stl_path + style->value("open_with_pix").toString();
    open_with_pix = stl_path + style->value("open_with_pix").toString();
    list_view_pix = stl_path + style->value("list_view_pix").toString();
    icon_view_pix = stl_path + style->value("icon_view_pix").toString();
    upper_dir_pix = stl_path + style->value("upper_dir_pix").toString();
    new_folder_pix = stl_path + style->value("folder_link_pix").toString();
    new_file_pix = stl_path + style->value("file_link_pix").toString();
    style->endGroup(); //Other
    style->beginGroup("Message");
    ok_button_pix_path = stl_path + style->value("ok_button_pix").toString();
    close_button_pix_path = stl_path + style->value("close_button_pix").toString();
    style->endGroup(); // Message
    style->beginGroup("Deskfolder");
    folder_pix = stl_path + style->value("d_folder_pix").toString();
    style->endGroup(); //Deskfolder
}

void Filedialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::darkGray, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawRect(0, 0, width(), height());
}

void Filedialog::init()
{
    message = new QLabel(this); // show message
    message->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    message->setMaximumHeight(20);

    QHBoxLayout *path_layout = new QHBoxLayout();
    line_path = new QLineEdit(this); // show path selection
    upper_dir_but = new QPushButton(QIcon(upper_dir_pix), "", this);
    upper_dir_but->setToolTip(tr("Upper directory"));
    path_layout->addWidget(line_path);
    path_layout->addWidget(upper_dir_but);

    QHBoxLayout *view_layout = new QHBoxLayout();
    QRadioButton *hidden_radio = new QRadioButton(tr("Show hidden files"), this);
    icon_but = new QPushButton(QIcon(icon_view_pix), tr("Icons"), this);
    list_but = new QPushButton(QIcon(list_view_pix), tr("List"), this);
    hidden_radio->setAutoExclusive(false);
    icon_but->setAutoExclusive(true);
    list_but->setAutoExclusive(true);
    view_layout->addWidget(hidden_radio);
    view_layout->addWidget(icon_but);
    view_layout->addWidget(list_but);
    view_layout->addStretch(1);

    fs_model = new QFileSystemModel(this);
    fs_model->setSupportedDragActions(Qt::LinkAction);
    fs_model->setFilter(fs_model->filter() | QDir::Hidden); // add hidden files to filter
    fs_model->setRootPath("/"); // must set on init

    QCompleter *completer = new QCompleter(this); // to complete the path
    completer->setModel(new QDirModel(completer));
    line_path->setCompleter(completer);

    prov = new Fileicon(); // get the files icon
    fs_model->setIconProvider(prov);

    tree_view = new QTreeView(this);
    tree_view->setModel(fs_model);
    tree_view->setDragEnabled(true);
    tree_view->viewport()->setAcceptDrops(true);
    tree_view->setDropIndicatorShown(true);
    tree_view->setDragDropMode(QAbstractItemView::DragDrop);
    tree_view->setItemsExpandable(false);
    tree_view->setRootIsDecorated(false);
    tree_view->setSortingEnabled(true);
    tree_view->header()->setSortIndicator(0, Qt::AscendingOrder);
    tree_view->setAlternatingRowColors(true);
    tree_view->setFocusPolicy(Qt::ClickFocus);
    tree_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree_view->setSelectionBehavior(QAbstractItemView::SelectItems);
    tree_view->setIconSize(QSize(16, 16));
    tree_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    list_view = new QListView(this);
    list_view->setModel(fs_model);
    list_view->setDragEnabled(true);
    list_view->viewport()->setAcceptDrops(true);
    list_view->setDropIndicatorShown(true);
    list_view->setDragDropMode(QAbstractItemView::DragDrop);
    list_view->setFlow(QListView::LeftToRight);
    list_view->setResizeMode(QListView::Adjust);
    list_view->setViewMode(QListView::IconMode);
    list_view->setUniformItemSizes(true);
    list_view->setFocusPolicy(Qt::ClickFocus);
    list_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    list_view->setSelectionBehavior(QAbstractItemView::SelectItems);
    list_view->setGridSize(QSize(70, 70));
    list_view->setSpacing(5);
    list_view->setIconSize(QSize(32, 32));
    list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    button_box = new QDialogButtonBox(this);
    ok = new QPushButton(QIcon(ok_button_pix_path), tr("Ok"));
    close = new QPushButton(QIcon(close_button_pix_path), tr("Close"));

    path_widget = new QListWidget(this);
    path_widget->setMaximumWidth(150);
    root_item = new QListWidgetItem(path_widget);
    bin_item = new QListWidgetItem(path_widget);
    home_item = new QListWidgetItem(path_widget);
    root_item->setIcon(QIcon(folder_pix));
    bin_item->setIcon(QIcon(folder_pix));
    home_item->setIcon(QIcon(folder_pix));
    root_item->setText(tr("/"));
    bin_item->setText(tr("/usr/bin/"));
    home_item->setText(tr("/home/"));

    QGroupBox *info_box = new QGroupBox(tr("Info"));
    info_box->setMaximumWidth(150);
    QGridLayout *info_layout = new QGridLayout();
    info_box->setLayout(info_layout);
    QLabel *owner = new QLabel(this);
    QLabel *permissions = new QLabel(this);
    preview_pix = new QLabel(this); // show pixmap preview
    owner_name = new QLabel(this); // show the file owner
    file_permissions = new QLabel(this); // show file permissions
    file_name = new QLabel(this); // show file name and extension
    preview_pix->setScaledContents(true);
    preview_pix->setFixedSize(32, 32);
    owner->setText(tr("<b>Owner:</b>"));
    permissions->setText(tr("<b>Permissions:</b>"));
    permissions->setWordWrap(true);
    file_name->setWordWrap(true);
    info_layout->addWidget(preview_pix, 0, 0, 1, 0, Qt::AlignCenter);
    info_layout->addWidget(file_name, 1, 0, 1, 0, Qt::AlignCenter);
    info_layout->addWidget(owner, 2, 0, Qt::AlignLeft);
    info_layout->addWidget(owner_name, 2, 1, Qt::AlignLeft);
    info_layout->addWidget(permissions, 3, 0, Qt::AlignLeft);
    info_layout->addWidget(file_permissions, 3, 1, Qt::AlignLeft);

    QSplitter *ver_splitter = new QSplitter(this);
    ver_splitter->setOrientation(Qt::Vertical);
    ver_splitter->addWidget(path_widget);
    ver_splitter->addWidget(info_box);

    QSplitter *hor_splitter = new QSplitter(this);
    hor_splitter->setOrientation(Qt::Horizontal);
    hor_splitter->addWidget(ver_splitter);
    hor_splitter->addWidget(tree_view);
    hor_splitter->addWidget(list_view);

    QVBoxLayout *layout = new QVBoxLayout();
    setLayout(layout);
    layout->addWidget(message);
    layout->addLayout(view_layout);
    layout->addLayout(path_layout);
    layout->addWidget(hor_splitter);
    layout->addWidget(button_box);

    connect(path_widget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this, SLOT(change_path(QListWidgetItem *, QListWidgetItem *)));
    connect(tree_view, SIGNAL(pressed(const QModelIndex &)), this, SLOT(show_info(const QModelIndex &)));
    connect(list_view, SIGNAL(pressed(const QModelIndex &)), this, SLOT(show_info(const QModelIndex &)));
    connect(tree_view, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(update_view(const QModelIndex &)));
    connect(list_view, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(update_view(const QModelIndex &)));
    connect(hidden_radio, SIGNAL(toggled(bool)), this, SLOT(show_hidden(bool)));
    connect(icon_but, SIGNAL(pressed()), this, SLOT(set_icon_mode()));
    connect(list_but, SIGNAL(pressed()), this, SLOT(set_list_mode()));
    connect(line_path, SIGNAL(returnPressed()), this, SLOT(path_completer()));
    connect(upper_dir_but, SIGNAL(pressed()), this, SLOT(upper_dir()));
    connect(button_box, SIGNAL(accepted()), this, SLOT(accepted()));
    connect(button_box, SIGNAL(rejected()), this, SLOT(rejected()));
}

void Filedialog::set_category_menu()
{
    // show the Category apps list for open the file
    main_menu = new QMenu(this);
    open_menu = main_menu->addMenu(QIcon(open_with_pix), tr("Open with"));

    menu_list = cat_menu->get_menus();

    for (int i = 0; i < menu_list.size(); ++i)
    {
        open_menu->addMenu(menu_list.at(i));
    }

    main_menu->addSeparator();
    QAction *del_act = main_menu->addAction(QIcon(delete_file_pix), tr("Delete"));
    cut_act = main_menu->addAction(QIcon(cut_file_pix), tr("Cut..."));
    copy_act = main_menu->addAction(QIcon(copy_file_pix), tr("Copy..."));
    paste_act = main_menu->addAction(QIcon(paste_file_pix), tr("Paste"));
    QAction *edit_act = main_menu->addAction(QIcon(edit_file_pix), tr("Rename"));
    main_menu->addSeparator();
    QAction *new_folder_act = main_menu->addAction(QIcon(new_folder_pix), tr("New folder"));
    QAction *new_file_act = main_menu->addAction(QIcon(new_file_pix), tr("New file"));

    reset_actions();

    connect(main_menu, SIGNAL(aboutToHide()), this, SLOT(reset_actions()));
    connect(del_act, SIGNAL(triggered()), this, SLOT(del_file()));
    connect(new_folder_act, SIGNAL(triggered()), this, SLOT(new_folder()));
    connect(new_file_act, SIGNAL(triggered()), this, SLOT(new_file()));
    connect(cut_act, SIGNAL(triggered()), this, SLOT(cut_file()));
    connect(copy_act, SIGNAL(triggered()), this, SLOT(copy_file()));
    connect(paste_act, SIGNAL(triggered()), this, SLOT(paste_file()));
    connect(edit_act, SIGNAL(triggered()), this, SLOT(edit_file()));
}

void Filedialog::reset_actions() // reset copy/paste action buttons
{
    cut_act->setDisabled(false);
    copy_act->setDisabled(false);
    paste_act->setDisabled(true);
}

void Filedialog::set_type(const QString &text, const QString &button_type) // set filedialog type
{
    message->setText(text); // show message

    if (button_type.compare("OK_Close") == 0)
    {
        button_box->addButton(ok, QDialogButtonBox::AcceptRole);
        button_box->addButton(close, QDialogButtonBox::RejectRole);
    }
    if (button_type.compare("Close") == 0)
    {
        button_box->removeButton(ok);
        button_box->addButton(close, QDialogButtonBox::RejectRole);
    }
}

void Filedialog::del_file()
{
    copy_list.clear();
    cut_list.clear();

    QList<QModelIndex> selection = abstract_view->selectionModel()->selectedIndexes();
    QModelIndex index;

    QString trash_path = QDir::homePath() + "/.local/share"; // search in default path directory

    foreach(index, selection)
    {
        abstract_view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent); // update the current index
        QString name = get_selected_name();
        QString path = get_selected_path();

        if (fs_model->isDir(index)) // test if is a directory
        {
            // create the .trashinfo file
            QString trash_info = name + ".trashinfo";
            QSettings settings(trash_path + "/Trash/info/" + trash_info, QSettings::IniFormat);
            settings.beginGroup("Trash Info");
            settings.setValue("Path", path);
            settings.setValue("DeletionDate", QDateTime::currentDateTime().toString(Qt::ISODate));
            settings.endGroup(); // Trash Info

            QStringList rem_info_args;
            rem_info_args << path << trash_path + "/Trash/files/";

            QProcess::startDetached("/bin/mv", rem_info_args); // remove the directory
        }
        else // else is a file
        {
            // create the .trashinfo file
            QString trash_info = name + ".trashinfo";
            QSettings settings(trash_path + "/Trash/info/" + trash_info, QSettings::IniFormat);
            settings.beginGroup("Trash Info");
            settings.setValue("Path", path + name);
            settings.setValue("DeletionDate", QDateTime::currentDateTime().toString(Qt::ISODate));
            settings.endGroup(); // Trash Info

            QStringList rem_info_args;
            rem_info_args << path + name << trash_path + "/Trash/files/";

            QProcess::startDetached("/bin/mv", rem_info_args); // remove the file
        }
    }
}

void Filedialog::cut_file()
{
    copy_list.clear();
    cut_list.clear();

    QList<QModelIndex> selection = abstract_view->selectionModel()->selectedIndexes();
    QModelIndex index;

    foreach(index, selection)
    {
        abstract_view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent); // update the current index
        QString name = get_selected_name();
        QString path = get_selected_path();

        if (fs_model->isDir(index)) // test if is a directory
        {
            cut_list << QString("/bin/mv").append(" ").append(path);
        }
        else // is a file
        {
            cut_list << QString("/bin/mv").append(" ").append(path).append(name);
        }
        cut_act->setDisabled(true);
        copy_act->setDisabled(true);
        paste_act->setDisabled(false);
    }
}

void Filedialog::copy_file()
{
    copy_list.clear();
    cut_list.clear();

    QList<QModelIndex> selection = abstract_view->selectionModel()->selectedIndexes();
    QModelIndex index;

    foreach(index, selection)
    {
        abstract_view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent); // update the current index
        QString name = get_selected_name();
        QString path = get_selected_path();

        if (fs_model->isDir(index)) // test if is a directory
        {
            copy_list << QString("/bin/cp -R").append(" ").append(path);
        }
        else // is a file
        {
            copy_list << QString("/bin/cp").append(" ").append(path).append(name);
        }
        cut_act->setDisabled(true);
        copy_act->setDisabled(true);
        paste_act->setDisabled(false);
    }
}

void Filedialog::paste_file()
{
    if (copy_list.size() > 0)
    {
        for (int i = 0; i < copy_list.size(); ++i)
        {
            QString command = QString(copy_list.at(i)).append(" ").append(line_path->text());
            qDebug() << "Paste command (copy):" << command;
            QProcess::startDetached(command);
        }
    }
    if (cut_list.size() > 0)
    {
        for (int i = 0; i < cut_list.size(); ++i)
        {
            QString command = QString(cut_list.at(i)).append(" ").append(line_path->text());
            qDebug() << "Paste command (cut):" << command;
            QProcess::startDetached(command);
        }
    }
    copy_list.clear();
    cut_list.clear();

    cut_act->setDisabled(false);
    copy_act->setDisabled(false);
    paste_act->setDisabled(true);
}

void Filedialog::edit_file()
{
    abstract_view->edit(abstract_view->selectionModel()->currentIndex());
}

void Filedialog::new_folder()
{
    fs_model->mkdir(fs_model->index(line_path->text()), tr("New folder"));
}

void Filedialog::new_file()
{
    QProcess::startDetached(QString("/bin/touch").append(" ").append(line_path->text()).append("/new_file"));
}

void Filedialog::change_path(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    abstract_view->setRootIndex(fs_model->index(current->text()));
    line_path->setText(current->text());
}

void Filedialog::set_path(const QString &pth) // for folder navigation
{
    abstract_view->setRootIndex(fs_model->index(pth));
    line_path->setText(pth);
}

void Filedialog::path_completer() // on user "return" press in line_path
{
    update_view(fs_model->index(line_path->text()));
}

void Filedialog::update_view(const QModelIndex &index)
{
    abstract_view->selectionModel()->clearSelection();
    abstract_view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
    line_path->setText(fs_model->filePath(index));
    
    if (fs_model->isDir(index)) // is a directory
    {
        abstract_view->setRootIndex(index);
    }
}

void Filedialog::show_info(const QModelIndex &index)
{
    if (index.isValid())
    {
        abstract_view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
        QString suff =  fs_model->fileInfo(index).suffix();
        if (suff == "png" || suff == "jpg")
            preview_pix->setPixmap(fs_model->filePath(index)); // show imagefile pixmap preview
        else
            preview_pix->setPixmap(fs_model->fileIcon(index).pixmap(32, 32)); // show file pixmap preview
        file_name->setText(fs_model->fileInfo(index).fileName());
        owner_name->setText(fs_model->fileInfo(index).owner());
        QString perm = QString("%1").arg(fs_model->fileInfo(index).permissions(), 0, 16);
        file_permissions->setText(perm);
    }
}

void Filedialog::upper_dir() // go to the upper directory
{
    update_view(fs_model->index(line_path->text()).parent());
}

void Filedialog::set_filter(QDir::Filters fltr)
{
    fs_model->setFilter(fltr);
}

void Filedialog::set_name_filters(const QStringList &fltr)
{
    fs_model->setNameFilters(fltr);
}

void Filedialog::set_read_only(bool b)
{
    fs_model->setReadOnly(b);
}

void Filedialog::show_hidden(bool select)
{
    if (select)
        fs_model->setFilter(fs_model->filter() | QDir::Hidden); // add hidden files to filter
    else
        fs_model->setFilter(fs_model->filter() ^ QDir::Hidden); // remove hidden files from filter
}

void Filedialog::set_icon_mode()
{
    list_view->show();
    tree_view->hide();
    abstract_view = list_view; // change the view
    abstract_view->setRootIndex(fs_model->index(line_path->text()));
}

void Filedialog::set_list_mode()
{
    list_view->hide();
    tree_view->show();
    abstract_view = tree_view; // change the view
    abstract_view->setRootIndex(fs_model->index(line_path->text()));
}

QString Filedialog::get_selected_path() const
{
    if (abstract_view->selectionModel()->currentIndex().isValid())
    {
        QModelIndex selection = abstract_view->selectionModel()->currentIndex();

        if (fs_model->isDir(selection)) // is a directory
        {
            QString path = fs_model->filePath(selection);
            qDebug() << "Selected path:" << path;
            return path;
        }
        else // is a file
        {
            QString filepath = fs_model->filePath(selection);
            QFileInfo fileinfo(filepath);
            QString path = fileinfo.absolutePath(); // remove the file name from path
            path.append("/"); // add slash at the end
            qDebug() << "Selected path:" << path;
            return path;
        }
    }
    return "";
}

QString Filedialog::get_selected_name() const
{
    if (abstract_view->selectionModel()->currentIndex().isValid())
    {
        QString name = fs_model->fileName(abstract_view->selectionModel()->currentIndex());
        qDebug() << "Selected name:" << name;
        return name;
    }
    return "";
}

QString Filedialog::get_selected_icon() const
{
    QString file = get_selected_name();
    QFileInfo fileinfo(file);
    Fileicon *prov = (Fileicon *)fs_model->iconProvider();
    QString icon = prov->icon_type(fileinfo); // get the file icon
    return icon;
}

void Filedialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mousepos = event->pos();
        grabMouse(QCursor(Qt::SizeAllCursor));
        raise();
    }
}

void Filedialog::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p = event->globalPos()-mousepos;
    move(p.x(), p.y());
}

void Filedialog::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    releaseMouse();
}

void Filedialog::contextMenuEvent(QContextMenuEvent *event)
{
    if (cat_menu != NULL)
    {
        QList<QModelIndex> selection = abstract_view->selectionModel()->selectedIndexes();

        if (selection.size() == 0 && cut_list.isEmpty() && copy_list.isEmpty())
        {
            event->ignore();
            return;
        }
        if (selection.size() > 1 || !cut_list.isEmpty() || !copy_list.isEmpty()) // don't show "Open with.." on multi selection
            // or during cut/paste
        {
            open_menu->setDisabled(true);
        }
        if (selection.size() == 1)
        {
            open_menu->setDisabled(false);
            QModelIndex index = selection.first(); // the unique selected index

            if (fs_model->isDir(index))
            {
                cat_menu->set_cmd_arguments(get_selected_path()); // set the dir path as argument
            }
            else
            {
                cat_menu->set_cmd_arguments(get_selected_path().append(get_selected_name())); // set the file path+name as argument
            }
        }
        main_menu->exec(event->globalPos()); // show the menu
        cat_menu->clear_cmd_arguments();
    }
    else
    {
        event->ignore();
        return;
    }
}

void Filedialog::accepted()
{
    accept();
    abstract_view->selectionModel()->clearSelection();
    set_path("/");
}

void Filedialog::rejected()
{
    reject();
    abstract_view->selectionModel()->clearSelection();
    set_path("/");
}

void Filedialog::update_style()
{
    read_settings();
    icon_but->setIcon(QIcon(icon_view_pix));
    list_but->setIcon(QIcon(list_view_pix));
    upper_dir_but->setIcon(QIcon(upper_dir_pix));
}
