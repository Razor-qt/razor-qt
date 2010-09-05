////////////////////////////////////////

//  File      : antico.h              //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// X event handler
// create new frame
// set window manager init settings to "razor.cfg"
// quit window manager

////////////////////////////////////////

#ifndef ANTICO_H
#define ANTICO_H

#include "defs.h"
#include "frame.h"
#include "dockbar.h"
#include "desk.h"
#include "filedialog.h"

class Dockbar;
class Frame;
class Desk;
class Filedialog;

////////////////////////////////////////

class Razor : public QApplication
{
    Q_OBJECT

protected:
    virtual bool x11EventFilter(XEvent *);

public:
    Razor(int &argc, char **argv);
    ~Razor();
    void create_frame(Window, Dockbar *, Desk *);
    void raise_next_frame();
    void set_active_frame(Frame *);
    void send_configurenotify(Frame *);
    void wm_quit();
    void wm_shutdown();
    void wm_restart();
    void wm_refresh();
    void show_desktop();
    void set_settings();
    void run_app_at_startup();
    void get_atoms();
    void create_gui();
    void set_event_names();
    void send_supported_hints();
    void check_window_type(Window);
    void print_window_prop(Window);
    bool check_net_sys_tray_for(Window) const;
    void check_wm_transient_for(Window);
    Filedialog * get_file_dialog();
    Desk * get_desktop();
    Dockbar * get_dockbar();
    Categorymenu * get_category_menu();
    
   
private:
    QHash<int, QString> event_names; // event names
    QHash<int, Frame *> mapping_clients; // mapping client and frame (key=client_win_id value=frame)
    QHash<int, Frame *> mapping_frames; // mapping frame with their winId (key=frame_win_id value=frame)
    QList<Frame *> frm_list;
    int next_frame;
    QStringList frame_type;
    int servershapes; // server supports shapes
    int ShapeEventBase; // event base for shape extension
    // get WM protocols required by ICCCM
    Atom wm_protocols;
    Atom wm_delete_window;
    Atom wm_change_state;
    Atom wm_state;
    Atom wm_take_focus;
    Atom wm_resource_manager;
    Atom wm_colormaps;
    Atom wm_hints;
    Atom wm_normal_hints;
    Atom wm_name;
    Atom wm_transient_for;
    // XDND Protocol
    Atom xdnd_aware;
    Atom xdnd_position;
    Atom xdnd_enter;
    Atom xdnd_finished;
    Atom xdnd_status;
    // Extensions
    Atom _net_wm_name;
    Atom _net_wm_icon;
    Atom _net_supported;
    Atom _net_wm_user_time;
    Atom _net_wm_state;
    Atom _net_wm_window_type;
    Atom _net_wm_window_type_normal;
    Atom _net_wm_window_type_desktop;
    Atom _net_wm_window_type_dialog;
    Atom _net_wm_window_type_splash;
    Atom _net_wm_window_type_dnd;
    Atom _kde_net_wm_system_tray_window_for;
    //////////////
    QSettings *antico;
    Dockbar *dock;
    Desk *dsk;
    Frame *frm;
    Categorymenu *cat_menu;
    Filedialog *file_dialog;
};
#endif
