////////////////////////////////////////
// File : dockbar.h                   //
// Written by: g_cigala@virgilio.it   //
// Copyright : GPL                    //
////////////////////////////////////////
 
// create the dockbar on bottom of the screen
 
////////////////////////////////////////
 
#ifndef DOCKBAR_H
#define DOCKBAR_H
 
#include "defs.h"
#include "launcher.h"
#include "systray.h"
#include "dockicon.h"
#include "dockapp.h"
#include "dockmenu.h"
#include "dateclock.h"
#include "filedialog.h"
 
class Frame;
class Dockapp;
class Dockicon;
class Systray;
class Razor;
class Launcher;
class Filedialog;
 
////////////////////////////////////////
 
class Dockbar : public QLabel
{
    Q_OBJECT
 
public:
    Dockbar(Razor *, QWidget *parent=0);
    ~Dockbar();
    void add_dockicon(Frame *);
    void remove_dockicon(Frame *);
    void update_dockicon_size();
    void read_settings();
    void init();
    void set_geometry();
    void update_style();
    void remove_dockicon(Window);
    void update_dockicon_name(const QString &, Frame *);
    void create_dock_app(const QString &, const QString &, QWidget *parent);
    void set_dockapp();
    void set_dockmenu();
    void set_favmenu();
    Systray *system_tray() const
    {
        return sys;
    }
 
public slots:
    void remove_dockicon(Dockicon *);
    void remove_dockapp(Dockapp *);
    void run_menu(QAction *);

 
protected:
    void mousePressEvent(QMouseEvent *);
    void dragEnterEvent(QDragEnterEvent *);
    void dragMoveEvent(QDragMoveEvent *);
    void dropEvent(QDropEvent *);
    
private:
    QHash<int, Dockicon *> dock_icons; // mapping Frame and Dockicon (key=frame win_id value=Dockicon)
    QList<Dockapp *> dock_apps;
    QList<Dockmenu *> dock_menus;
    int d_length;   //dockicon lenght
    int dock_height;
    int dock_width;
    int dock_position;
    QString dock_pix;
    QString arrow_pix;
    QString app_link_pix;
    QSettings *antico;
    QMenu *menu;
    QHBoxLayout *dock_layout;
    QHBoxLayout *icon_layout;
    QHBoxLayout *app_layout;
    QHBoxLayout *menu_layout;
    QHBoxLayout *favmenu_layout;
    QWidget *d_app_widget;
    QWidget *d_icon_widget;
    QWidget *d_menu_widget;
    QWidget *d_favmenu_widget;
    Launcher *lchr;
    Dockicon *d_icon;
    Systray *sys;
    Dateclock *clk;
    Razor *app;
    Filedialog *file_dialog;
 
};
#endif
