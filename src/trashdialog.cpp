////////////////////////////////////////
//  File      : trashdialog.cpp       //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "trashdialog.h"

////////////////////////////////////////

Trashdialog::Trashdialog(QWidget *parent) : QDialog(parent)
{
    setSizeGripEnabled(true);
    setWindowModality(Qt::WindowModal);
    read_settings();
    init();
}

Trashdialog::~Trashdialog()
{
    delete prov;
}

void Trashdialog::read_settings()
{
    // get style path
    QSettings *antico = new QSettings(QDir::homePath() + "/.razor/razor.cfg", QSettings::IniFormat, this);
    antico->beginGroup("Style");
    stl_name = antico->value("name").toString();
    stl_path = antico->value("path").toString();
    antico->endGroup(); //Style
    // get style values
    QSettings *style = new QSettings(stl_path + stl_name, QSettings::IniFormat, this);
    style->beginGroup("Message");
    ok_button_pix = stl_path + style->value("ok_button_pix").toString();
    close_button_pix = stl_path + style->value("close_button_pix").toString();
    restore_button_pix = stl_path + style->value("restore_button_pix").toString();
    remove_button_pix = stl_path + style->value("remove_button_pix").toString();
    style->endGroup(); //Message
}

void Trashdialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::darkGray, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawRect(0, 0, width(), height());
}

void Trashdialog::init()
{
    QVBoxLayout *trash_layout = new QVBoxLayout();
    setLayout(trash_layout);
    QLabel *name = new QLabel(tr("TRASH"), this);
    name->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    line_path = new QLineEdit(this); // show trash path
    line_path->setReadOnly(true);
    fs_model = new QFileSystemModel(this);
    prov = new Fileicon(); // get the files icon
    fs_model->setIconProvider(prov);
    fs_model->setFilter(fs_model->filter() | QDir::Hidden); // add hidden files to filter
    tree_view = new QTreeView(this);
    tree_view->setModel(fs_model);
    tree_view->setSortingEnabled(true);
    tree_view->setAlternatingRowColors(true);
    tree_view->setFocusPolicy(Qt::ClickFocus);
    tree_view->header()->setSortIndicator(0, Qt::AscendingOrder);
    tree_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree_view->setSelectionBehavior(QAbstractItemView::SelectItems);
    tree_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    trash_path = QDir::homePath() + "/.local/share"; // search in default path directory
    line_path->setText(trash_path + "/Trash/files");
    fs_model->setRootPath(trash_path + "/Trash/files");
    tree_view->setRootIndex(fs_model->index(trash_path + "/Trash/files"));
    QHBoxLayout *button_layout = new QHBoxLayout();
    QPushButton* delete_but = new QPushButton(QIcon(remove_button_pix), tr("Delete"), this);
    QPushButton* restore_but = new QPushButton(QIcon(restore_button_pix), tr("Restore"), this);
    QPushButton* close_but = new QPushButton(QIcon(close_button_pix), tr("Close"), this);
    button_layout->addWidget(delete_but);
    button_layout->addWidget(restore_but);
    button_layout->addWidget(close_but);
    connect(delete_but, SIGNAL(pressed()), this, SLOT(delete_pressed()));
    connect(restore_but, SIGNAL(pressed()), this, SLOT(restore_pressed()));
    connect(close_but, SIGNAL(pressed()), this, SLOT(close_pressed()));
    trash_layout->addWidget(name);
    trash_layout->addWidget(line_path);
    trash_layout->addWidget(tree_view);
    trash_layout->addLayout(button_layout);
}

void Trashdialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mousepos = event->pos();
        grabMouse(QCursor(Qt::SizeAllCursor));
        raise();
    }
}

void Trashdialog::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p = event->globalPos()-mousepos;
    move(p.x(), p.y());
}

void Trashdialog::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    releaseMouse();
}

void Trashdialog::close_pressed()
{
    hide();
}

void Trashdialog::delete_pressed()
{
    QList<QModelIndex> selection = tree_view->selectionModel()->selectedIndexes();
    QModelIndex index;

    foreach(index, selection)
    {
        tree_view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent); // update the current index

        if (index.isValid())
        {
            QString selection_path = fs_model->filePath(tree_view->currentIndex());
            QString selection_name = fs_model->fileName(tree_view->currentIndex());
            qDebug() << "Selected file path:" << selection_path;
            QString trash_info = selection_name + ".trashinfo";
            QStringList rem_info_args;
            rem_info_args <<  trash_path + "/Trash/info/" + trash_info;
            QProcess::startDetached("/bin/rm", rem_info_args); // remove the info file
            QStringList rem_file_args;
            rem_file_args << "-rf" << selection_path;
            QProcess::startDetached("/bin/rm", rem_file_args); // remove the selected dir/file
        }
    }
    Msgbox msg;
    msg.set_header(tr("INFORMATION"));
    msg.set_info("Files definitively deleted...");
    msg.set_icon("Information");
    msg.exec();
}

void Trashdialog::restore_pressed()
{
    QList<QModelIndex> selection = tree_view->selectionModel()->selectedIndexes();
    QModelIndex index;

    foreach(index, selection)
    {
        tree_view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent); // update the current index

        if (index.isValid())
        {
            QString selection_path = fs_model->filePath(tree_view->currentIndex());
            QString selection_name = fs_model->fileName(tree_view->currentIndex());
            qDebug() << "Selected file path:" << selection_path;
            QString trash_info = selection_name + ".trashinfo";
            QSettings settings(trash_path + "/Trash/info/" + trash_info, QSettings::IniFormat);
            settings.beginGroup("Trash Info");
            QString restore_full_path = settings.value("Path").toString(); // with file name at end
            settings.endGroup(); // Trash Info
            qDebug() << "Restore full path:" << restore_full_path;
            QString restore_path = restore_full_path.remove(selection_name); // remove the file name from restore path
            qDebug() << "Restore path:" << restore_path;
            QStringList restore_args;
            restore_args << selection_path << restore_path;
            QProcess::startDetached("/bin/mv", restore_args); // restore the selected dir/file
            QStringList remove_args;
            remove_args <<  trash_path + "/Trash/info/" + trash_info;
            QProcess::startDetached("/bin/rm", remove_args); // remove the info file

            Msgbox msg;
            msg.set_header(tr("INFORMATION"));
            msg.set_info("<b>" + selection_name + "</b>" + " " + tr("will be restored in") + " " + "<b>" + restore_path + "</b>");
            msg.set_icon("Information");
            msg.exec();
        }
    }
}

int Trashdialog::files_counter()
{
    int counter = fs_model->rootDirectory().count();
    return counter;
}
