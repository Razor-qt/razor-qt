////////////////////////////////////////
//  File      : msgbox.h              //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// information box

////////////////////////////////////////

#ifndef MSGBOX_H
#define MSGBOX_H

#include "defs.h"

////////////////////////////////////////

class Msgbox : public QDialog
{
    Q_OBJECT

public:
    Msgbox(QWidget *parent=0);
    ~Msgbox();
    void init();
    void read_settings();
    void set_header(const QString &);
    void set_info(const QString &);
    void set_icon(const QString &);

protected:
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);

signals:

public slots:

private:
    QPoint mousepos;
    QString ok_button_pix_path;
    QString close_button_pix_path;
    QString add_button_pix_path;
    QString remove_button_pix_path;
    QString question_pix_path;
    QString information_pix_path;
    QString warning_pix_path;
    QString critical_pix_path;
    QLabel *msg_header;
    QLabel *msg_info;
    QLabel *msg_icon;
    QDialogButtonBox *button_box;
    QPushButton *ok_but;
    QPushButton *close_but; 
};


#endif
