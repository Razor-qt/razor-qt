////////////////////////////////////////
//  File      : border.h              //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// create the frame border

////////////////////////////////////////

#ifndef BORDER_H
#define BORDER_H

#include "defs.h"

////////////////////////////////////////

class Border : public QLabel
{
    Q_OBJECT

public:
    Border(QWidget *parent=0);
    ~Border();

signals:
    void mouse_move(QMouseEvent *);
    void mouse_left_press(QMouseEvent *);
    void mouse_left_press();
    void mouse_right_press();
    void mouse_mid_press();

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:

};

#endif
