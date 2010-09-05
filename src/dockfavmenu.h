#ifndef DOCKFAVMENU_H
#define DOCKFAVMENU_H
////////////////////////////////////////
//  File      : dockfavmenu.h         //
//  Written by: chris.vdop@gmail.com  //
//  Copyright : GPL                   //
// This is a single menu-bar launcher //
// for the dockbar of razor			  //
////////////////////////////////////////
#include "defs.h"



class Dockfavmenu : public QWidget
{
    Q_OBJECT
 
public:
    Dockfavmenu(QMenu *menu, QWidget *parent=0);
    ~Dockfavmenu();
    void read_settings();
    void update_style();

protected:
    void mousePressEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void paintEvent(QPaintEvent *);
 
public slots:

signals:
 
private:
    QString d_menu_pix;
    QColor title_color;
    QPixmap back_pix;
    QPixmap cat_pix;
    int bdr_width;
    int dock_position;
    bool zoom;
    QMenu *cat_menu;
};
 
#endif