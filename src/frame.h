////////////////////////////////////////
//  File      : frame.h               //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// create window frame around X11 apps
// manage all the move/resize event

////////////////////////////////////////

#ifndef FRAME_H
#define FRAME_H

#define TILE_NONE		0
#define TILE_HALF_LEFT 		1
#define TILE_HALF_RIGHT 	2
#define TILE_QUART_LEFT_UP 	3
#define TILE_QUART_LEFT_DOWN 	4
#define TILE_QUART_RIGHT_UP 	5
#define TILE_QUART_RIGHT_DOWN 	6




#include "defs.h"
#include "border.h"
#include "header.h"
#include "desk.h"
#include "dockbar.h"
class Desk;
class Dockbar;
class Header;

////////////////////////////////////////

class Frame : public QFrame
{
    Q_OBJECT

public:
    Frame(Window, const QString &, Dockbar *, Desk *, QWidget *parent=0);
    ~Frame();
    void init();
    void update_style();
    void create_borders();
    void get_client_geometry();
    void set_frame_geometry();
    void set_window_modality();
    void get_wm_hints();
    void get_wm_normal_hints();
    void unmap_it();
    void withdrawn_it();
    void map_it();
    void raise_it();
    void reshape();
    bool query_shape() const;
    void get_wm_name();
    void update_name();
    void get_wm_protocols();
    void send_wm_protocols(long, long);
    void resize_request(int, int);
    void set_active();
    void set_inactive();
    void set_state(int);
    void set_focus(long);
    void get_icon();
    void get_colormaps();
    void set_colormaps(Colormap);
    void read_settings(); // from "razor.cfg"
    int get_window_middle_x();
    int get_window_middle_y();
    WId cl_win()
    {
        return(c_win);
    }
    int cl_x()
    {
        return(x()+lateral_bdr_width);
    }
    int cl_y()
    {
        return(y()+top_bdr_height);
    }
    int cl_h()
    {
        return(height()-diff_border_h);
    }
    int cl_w()
    {
        return(width()-diff_border_w);
    }
    QString cl_name()
    {
        return(wm_name);
    }
    QPixmap cl_icon()
    {
        return(wicon);
    }
    QString win_state()
    {
        return state;
    }
    bool is_splash()
    {
        return splash;
    }

public slots:
    void press_top_mid(QMouseEvent *);          // top mid border press
    void move_top_mid(QMouseEvent *);           // top mid border move
    void press_bottom_left(QMouseEvent *);      // bottom left border press
    void move_bottom_left(QMouseEvent *);       // bottom left border move
    void press_bottom_right(QMouseEvent *);     // bottom right border press
    void move_bottom_right(QMouseEvent *);      // bottom right border move
    void press_bottom_mid(QMouseEvent *);       // bottom mid border press
    void move_bottom_mid(QMouseEvent *);        // bottom mid border move
    void press_right(QMouseEvent *);            // right border press
    void move_right(QMouseEvent *);             // right border move
    void press_left(QMouseEvent *);             // left border press
    void move_left(QMouseEvent *);              // left border move
    void destroy_it();                          // destroy client
    void maximize_it();                         // maximize client
    void iconify_it();                          // iconify client
    void tile_it();				// Used for tiling to full height
    void tile_it_hhl();				// Used for tiling to half height left screenpart
    void tile_it_hhr();				// Used for tiling to half height right screenpart
    
    void dragEnterEvent(QDragEnterEvent *);
    void dragMoveEvent(QDragMoveEvent *);
    void dropEvent(QDropEvent *);
    
private:
    int tile;			// tiling status of the window
    WId c_win;                  // client window
    QString frame_type;         // frame type (Normal, Splash, Dialog, Desktop..)
    QString app_icon;           // default header icon used if no icon is find
    QPoint mousepos;            // mouse position at button press
    int m_px, m_py, m_pw, m_ph; // normal parent geometry in maximize state
    int n_px, n_py, n_pw, n_ph; // normal parent geometry in normal mode
    int frame_w, frame_h;       // frame geometry
    int frame_x, frame_y;       // frame geometry
    int client_x, client_y;     // client geometry
    int client_w, client_h;     // client geometry
    int lateral_bdr_width;      // parent lateral border width
    int top_bdr_height;         // parent top border height
    int bottom_bdr_height;      // parent top border height
    int win_gravity;            // client gravity
    QString header_active_pix;  // frame header pixmap
    QString header_inactive_pix;// frame header pixmap
    QString minmax_pix;         // frame minmax pixmap
    QString close_pix;          // frame close pixmap
    QString arrow_pix;          // frame cursor
    QString move_pix;           // frame cursor
    QString left_pix;           // frame cursor
    QString right_pix;          // frame cursor
    QString bottom_pix;         // frame cursor
    QString bottom_left_pix;    // frame cursor
    QString bottom_right_pix;   // frame cursor
    QColor title_color;         // frame title color
    int diff_border_h;          // height space between parent frame (qt) and client frame
    int diff_border_w;          // width space between parent frame (qt) and client frame
    int dock_height;            // dockbar height
    int dock_position;          // dockbar position (top, bottom)
    bool maximized;             // maximize window
    bool splash;                // splash window
    bool shaped;                // nonrectangular window
    QString state;              // window state (Normal, Iconic, Withdrawn)
    bool prot_delete;           // client has delete WM protocol
    bool prot_take_focus;       // client has take focus WM protocol
    QPixmap wicon;              // window icon
    QString wm_name;            // WM_NAME property or res_name
    QString res_name;           // ClassHint
    QString res_class;          // ClassHint
    bool inputfield;            // WMHints
    Colormap cmap;              // colormap
    Desk *desktop;              // desktop
    Dockbar *dockbar;           // dockbar
    Header *tm_bdr;             // top mid window border (for window move)
    Border *tl_bdr;             // top left window border (icon)
    Border *tr_bdr;             // top right window border (icon)
    Border *bm_bdr;             // bottom mid window border
    Border *bl_bdr;             // bottom left window border
    Border *br_bdr;             // bottom right window border
    Border *l_bdr;              // left window border
    Border *r_bdr;              // right window border
    Border *c_bdr;              // center window border (client apps)
    QGridLayout *layout;
    QSettings *antico;
};
#endif
