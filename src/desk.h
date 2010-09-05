////////////////////////////////////////
//  File      : desk.h                //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// the desktop on the screen

////////////////////////////////////////

#ifndef DESK_H
#define DESK_H

#include "defs.h"
#include "deskfolder.h"
#include "deskfile.h"
#include "deskapp.h"
#include "deskdev.h"
#include "deskicon.h"
#include "msgbox.h"
#include "antico.h"
#include "filedialog.h"
#include "trash.h"
#include "utils.h"
#include "frame.h"

class Razor;
class Deskapp;
class Deskdev;
class Deskfile;
class Deskfolder;
class Deskicon;
class Filedialog;
class Frame;

////////////////////////////////////////

class Desk : public QLabel
{
    Q_OBJECT

public:
    Desk(Razor *, QWidget *parent=0);
    ~Desk();
    void init();
    void read_settings();
    void set_geometry();
    void set_desk_icons();
    void create_desk_app(const QString &, const QString &, const QString &, const QPoint &, QWidget *);
    void create_desk_file(const QString &, const QString &, const QString &, const QPoint &, QWidget *);
    void create_desk_folder(const QString &, const QString &, const QRect &, const QPoint &, QWidget *);
    void add_deskicon(Frame *);
    void remove_deskicon(Frame *);
    void remove_deskicon(Window);
    void map_deskicon(Frame *);
    void unmap_deskicon(Frame *);
    void update_style();
    void mount_device(const QString &, const QString &, const QString &, const QString &, const QString &);
    void unmount_device(const QString &);
  
public slots:
    void run_menu(QAction *);
    void remove_deskfolder(Deskfolder *);
    void remove_deskfile(Deskfile *);
    void remove_deskapp(Deskapp *);
    void remove_deskicon(Deskicon *);
    void device_added( const QString &);
    void device_removed(const QString &);

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);
    void dragEnterEvent(QDragEnterEvent *);
    void dragMoveEvent(QDragMoveEvent *);
    void dropEvent(QDropEvent *);

private:
    int dock_height;
    QList<Deskfolder *> desk_folders;
    QList<Deskfolder *> desk_folders_selected;
    QList<Deskfile *> desk_files;
    QList<Deskfile *> desk_files_selected;
    QList<Deskapp *> desk_apps;
    QList<Deskapp *> desk_apps_selected;
    QHash<QString, Deskdev *> desk_dev;
    QHash<int, Deskicon *> desk_icons; // mapping Frame and Deskicon (key=frame win_id value=Deskicon)
    QString wall_pix;
    QString folder_link_pix;
    QString file_link_pix;
    QString app_link_pix;
    QPoint rubber_press;
    QDBusInterface *dbus_interface;
    QRubberBand *rubber_band;
    QMenu *menu;
    QSettings *antico;
    Razor *app;
    Filedialog *file_dialog;
    Categorymenu *cat_menu;
    Deskfolder *d_folder;
    Deskfile *d_file;
    Deskapp *d_app;
    Deskicon *d_icon;
    Appicon *app_icon;
    Trash *trsh;
};


#endif
