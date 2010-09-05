////////////////////////////////////////
//  File      : manager.h             //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// manage the frame border, color, pixmap, desk wallpaper
// add/remove apps from the launcher menu

////////////////////////////////////////

#ifndef MANAGER_H
#define MANAGER_H

#include "defs.h"
#include "filedialog.h"
#include "colordialog.h"
#include "utils.h"

class Filedialog;

////////////////////////////////////////

class Manager : public QDialog
{
    Q_OBJECT

public:
    Manager(QWidget *parent=0);
    ~Manager();
    void init();
    void read_settings();
    void style_tab();
    void add_app_tab();
    void run_app_tab();
    void remove_app_tab();
    void update_remove_list();
    void update_run_table();

public slots:
    void ok_frame_pressed();
    void add_app_pressed();
    void add_run_app_pressed();
    void remove_app_pressed();
    void remove_run_app_pressed(); 
    void close_pressed();
    void select_pixmap(QWidget *);
    void select_color(QWidget *);
    void show_path(const QModelIndex &);
    void path_completer();
    void select_style();
    void changeSection(QListWidgetItem *, QListWidgetItem *);
    void change_path(QListWidgetItem *, QListWidgetItem *);
    void set_run_args(QTableWidgetItem *);

protected:
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);

private:
    QSpinBox *top_bdr_spinBox;
    QSpinBox *lateral_bdr_spinBox;
    QSpinBox *bottom_bdr_spinBox;
    QSpinBox *dockbar_height_spinBox;
    QSpinBox *dockbar_width_spinBox;
    QComboBox *dockbar_position_comboBox;
    QLabel *dockbar_pix;
    QLabel *deskfolder_pix;
    QLabel *deskdev_disk_pix;
    QLabel *deskdev_cdrom_pix;
    QLabel *trash_pix;
    QLabel *dockicon_pix;
    QLabel *sysicon_pix;
    QLabel *desktop_pix;
    QLabel *header_active_pix;
    QLabel *header_inactive_pix;
    QLabel *minmax_pix;
    QLabel *close_pix;
    /// Launcher ///
    QLabel *launcher_pix;
    QLabel *quit_pix;
    QLabel *shutdown_pix;
    QLabel *restart_pix;
    QLabel *refresh_pix;
    QLabel *run_pix;
    QLabel *show_pix;
    QLabel *manager_pix;
    QLabel *settings_pix;
    QLabel *utility_pix;
    QLabel *office_pix;
    QLabel *network_pix;
    QLabel *graphics_pix;
    QLabel *development_pix;
    QLabel *system_pix;
    QLabel *audiovideo_pix;
    /// Message ///
    QLabel *ok_button_pix;
    QLabel *close_button_pix;
    QLabel *add_button_pix;
    QLabel *remove_button_pix;
    QLabel *restore_button_pix;
    QLabel *question_pix;
    QLabel *information_pix;
    QLabel *warning_pix;
    QLabel *critical_pix;
    /// Other ///
    QLabel *application_pix;
    QLabel *folder_link_pix;
    QLabel *file_link_pix;
    QLabel *app_link_pix;
    QLabel *delete_link_pix;
    QLabel *delete_file_pix;
    QLabel *cut_file_pix;
    QLabel *copy_file_pix;
    QLabel *paste_file_pix;
    QLabel *close_dock_pix;
    QLabel *add_to_sys_pix;
    QLabel *open_with_pix;
    QLabel *list_view_pix;
    QLabel *icon_view_pix;
    QLabel *upper_dir_pix;

    QString dockbar_pix_path;
    QString deskfolder_pix_path;
    QString deskdev_disk_pix_path;
    QString deskdev_cdrom_pix_path;
    QString trash_pix_path;
    QString dockicon_pix_path;
    QString sysicon_pix_path;
    QString desktop_pix_path;
    QString header_active_pix_path;
    QString header_inactive_pix_path;
    QString minmax_pix_path;
    QString close_pix_path;
    /// Launcher ///
    QString launcher_pix_path;
    QString quit_pix_path;
    QString shutdown_pix_path;
    QString restart_pix_path;
    QString refresh_pix_path;
    QString run_pix_path;
    QString show_pix_path;
    QString manager_pix_path;
    QString settings_pix_path;
    QString utility_pix_path;
    QString office_pix_path;
    QString network_pix_path;
    QString graphics_pix_path;
    QString development_pix_path;
    QString system_pix_path;
    QString audiovideo_pix_path;
    /// Message ///
    QString ok_button_pix_path;
    QString close_button_pix_path;
    QString add_button_pix_path;
    QString remove_button_pix_path;
    QString restore_button_pix_path;
    QString question_pix_path;
    QString information_pix_path;
    QString warning_pix_path;
    QString critical_pix_path;
    /// Other ///
    QString application_pix_path;
    QString folder_link_pix_path;
    QString file_link_pix_path;
    QString app_link_pix_path;
    QString delete_link_pix_path;
    QString delete_file_pix_path;
    QString cut_file_pix_path;
    QString copy_file_pix_path;
    QString paste_file_pix_path;
    QString close_dock_pix_path;
    QString add_to_sys_pix_path;
    QString open_with_pix_path;
    QString list_view_pix_path;
    QString icon_view_pix_path;
    QString upper_dir_pix_path;
   
    QString stl_name;
    QString stl_path;
    QLabel *header_col_lab;
    QLabel *dockicon_col_lab;
    QLabel *deskfolder_col_lab;
    QLabel *deskfile_col_lab;
    QLabel *deskdev_col_lab;
    QLabel *trash_col_lab;
    QLabel *deskapp_col_lab;
    QLabel *date_col_lab;
    QLabel *clock_col_lab;
    QLineEdit *style_sel_tx;
    QString man_ico;
    QPoint mousepos;
    QTreeView *tree_view;
    QTreeWidget *app_tree;
    QTableWidget *run_table;
    QTabWidget *tab;
    QVBoxLayout *main_layout;
    QLineEdit *app_path;
    QListWidgetItem *root_item;
    QListWidgetItem *bin_item;
    QListWidgetItem *home_item;
    QListWidget *path_widget;
    
    QSettings *style;
    QSettings *antico;
    QVBoxLayout *add_layout;
    QVBoxLayout *rem_layout;
    QVBoxLayout *run_layout;
    QVBoxLayout *style_layout;
    QHBoxLayout *ok_quit_layout;
    QHBoxLayout *rem_quit_layout;
    QGridLayout *dockbar_layout;
    QGridLayout *deskset_layout;
    QListWidget *sections_widget;
    QStackedWidget *params_widget;
    QGroupBox *frame_box;
    QGroupBox *dockbar_box;
    QGroupBox *dockicon_box;
    QGroupBox *deskfolder_box;
    QGroupBox *deskfile_box;
    QGroupBox *deskapp_box;
    QGroupBox *deskdev_box;
    QGroupBox *trash_box;
    QGroupBox *sysicon_box;
    QGroupBox *dateclock_box;
    QGroupBox *desktop_box;
    QGroupBox *launcher_box;
    QGroupBox *category_box;
    QGroupBox *message_box;
    QGroupBox *other_box;
    QWidget *dockbar_widget;
    QWidget *deskset_widget;
    QGridLayout *style_grid;
    QGridLayout *frame_grid;
    QGridLayout *dockbar_grid;
    QGridLayout *dockicon_grid;
    QGridLayout *deskfolder_grid;
    QGridLayout *deskfile_grid;
    QGridLayout *deskdev_grid;
    QGridLayout *trash_grid;
    QGridLayout *sysicon_grid;
    QGridLayout *deskapp_grid;
    QGridLayout *dateclock_grid;
    QGridLayout *desktop_layout;
    QGridLayout *launcher_grid;
    QGridLayout *category_grid;
    QGridLayout *message_grid;
    QGridLayout *other_grid;
    QFileSystemModel *fs_model;
    QHBoxLayout *category_lay;
    QComboBox *category_combo;
    QSignalMapper *pixmapMapper;
    QSignalMapper *colorMapper;
    QTreeWidgetItem *category;
    QTreeWidgetItem *app;
    QLabel *color_lab;
    QLabel *pixmap;
    QMap <QString, QString> cat_map;
    Filedialog *file_dialog;
    Fileicon *prov;
    Appicon *app_ico;
};
#endif
