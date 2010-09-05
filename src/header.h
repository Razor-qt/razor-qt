////////////////////////////////////////
//  File      : header.h              //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// the header of the window frame (top mid border)

////////////////////////////////////////

#ifndef HEADER_H
#define HEADER_H

#include "defs.h"
#include "frame.h"

////////////////////////////////////////

class Header : public QWidget
{
    Q_OBJECT

public:
    Header(const QPixmap &, const QString &, QWidget *parent=0);
    ~Header();
    void set_pixmap(const QPixmap &, const QPixmap &, const QColor &);
    void set_active();
    void set_inactive();
    void update_name(const QString &);

signals:
    void mouse_release(QMouseEvent *);
    void mouse_move(QMouseEvent *);
    void mouse_left_press(QMouseEvent *);
    void mouse_left_press();
    void mouse_right_press();
    void mouse_mid_press();
    void mouse_double_click();

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);

private:
    QPixmap active_back;
    QPixmap inactive_back;
    QPixmap icon;
    QPixmap back;
    QString app_name;
    QColor title_color;
    bool active_state;
};

#endif
