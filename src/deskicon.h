////////////////////////////////////////
//  File      : deskicon.h            //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// add the application icon (small pixmap) on desktop

////////////////////////////////////////

#ifndef DESKICON_H
#define DESKICON_H

#include "frame.h"

class Frame;

////////////////////////////////////////

class Deskicon : public QLabel
{
    Q_OBJECT

public:
    Deskicon(Frame *, QWidget *parent=0);
    ~Deskicon();
    void read_settings();
   
protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

public slots:
    void run_menu(QAction *);
 
signals:
    void destroy_deskicon(Deskicon *);

private:
    QPoint mousepos;
    QPixmap grab_pix; 
    QString close_desk_pix;
    Frame *frm;
};

#endif
