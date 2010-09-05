////////////////////////////////////////
//  File      : filedialog.h          //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// dialog for directory selection

////////////////////////////////////////

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include "defs.h"
#include "msgbox.h"
#include "utils.h"
#include "header.h"

////////////////////////////////////////

class Filedialog : public QDialog
{
    Q_OBJECT

public:
    Filedialog(Categorymenu *, QWidget *parent=0);
    Filedialog(QWidget *parent=0);
    ~Filedialog();
    void init();
    void set_path(const QString &path);
    void set_type(const QString &, const QString &);
    QString get_selected_path() const;
    QString get_selected_name() const;
    QString get_selected_icon() const;
    void set_filter(QDir::Filters);
    void set_name_filters(const QStringList &fltr);
    void set_read_only(bool);
    void read_settings();
    void set_category_menu();
    void update_style();
   
protected:
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
    
signals:

public slots:
    void del_file();
    void cut_file();
    void copy_file();
    void paste_file();
    void edit_file();
    void new_folder();
    void new_file();
    void reset_actions();
    void show_hidden(bool);
    void path_completer();
    void upper_dir();
    void update_view(const QModelIndex &);
    void contextMenuEvent(QContextMenuEvent *);
    void change_path(QListWidgetItem *, QListWidgetItem *);
    void show_info(const QModelIndex &);
    void set_icon_mode();
    void set_list_mode();
    void accepted();
    void rejected();

private:
    QList <QMenu *> menu_list;
    QPoint mousepos;
    QString delete_file_pix;
    QString cut_file_pix;
    QString copy_file_pix;
    QString paste_file_pix;
    QString edit_file_pix;
    QString open_with_pix;
    QString list_view_pix;
    QString icon_view_pix;
    QString upper_dir_pix;
    QString new_folder_pix;
    QString new_file_pix;
    QString ok_button_pix_path;
    QString close_button_pix_path;
    QString folder_pix;
    QStringList cut_list;
    QStringList copy_list;
    QLabel *message;
    QLabel *preview_pix;
    QLabel *file_name;
    QLabel *owner_name;
    QLabel *file_permissions;
    QDialogButtonBox *button_box;
    QPushButton *ok;
    QPushButton *cancel;
    QPushButton *close;
    QSettings *antico;
    QListWidgetItem *root_item;
    QListWidgetItem *bin_item;
    QListWidgetItem *home_item;
    QListWidget *path_widget;
    QAction *cut_act;
    QAction *copy_act;
    QAction *paste_act;
    QFileSystemModel *fs_model;
    QListView *list_view;
    QTreeView *tree_view;
    QAbstractItemView *abstract_view;
    QLineEdit *line_path;
    QPushButton *upper_dir_but;
    QPushButton *icon_but;
    QPushButton *list_but;
    QMenu *main_menu;
    QMenu *open_menu;
    Categorymenu *cat_menu;
    Fileicon *prov;
};


#endif
