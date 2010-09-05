////////////////////////////////////////
// File : dockmenu.h                  //
// Written by: g_cigala@virgilio.it   //
// Copyright : GPL                    //
////////////////////////////////////////
 
// category menu on dockbar
 
////////////////////////////////////////
 
#ifndef DOCKMENU_H
#define DOCKMENU_H

#include "defs.h"

////////////////////////////////////////
 
class Dockmenu : public QWidget
{
    Q_OBJECT
 
public:
    Dockmenu(QMenu *menu, QWidget *parent=0);
    ~Dockmenu();
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