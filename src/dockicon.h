////////////////////////////////////////
// File : dockicon.h                  //
// Written by: g_cigala@virgilio.it   //
// Copyright : GPL                    //
////////////////////////////////////////
 
// app iconify added on dockbar
 
////////////////////////////////////////
 
#ifndef DOCKICON_H
#define DOCKICON_H
 
#include "frame.h"
#include "systray.h"
 
class Frame;
class Systray;
 
////////////////////////////////////////
 
class Dockicon : public QWidget
{
    Q_OBJECT
 
public:
    Dockicon(Frame *, Systray *, QWidget *parent=0);
    ~Dockicon();
    void read_settings();
    void update_style();
    void update_name(const QString &);

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void paintEvent(QPaintEvent *);
 
public slots:
    void run_menu(QAction *);
 
signals:
    void destroy_dockicon(Dockicon *);
 
private:
    QColor title_color;
    QString title;
    QString close_dock_pix;
    QString add_to_sys_pix;
    QPixmap pix;
    int bdr_width;
    int frame_state;
    Frame *frm;
    Systray *sys;
};
 
#endif
