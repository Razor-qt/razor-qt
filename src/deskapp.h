////////////////////////////////////////
//  File      : deskapp.h             //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// add application link on the desk

////////////////////////////////////////

#ifndef DESKAPP_H
#define DESKAPP_H

#include "defs.h"
#include "filedialog.h"

////////////////////////////////////////

class Deskapp : public QWidget
{
    Q_OBJECT

public:
    Deskapp(const QString &, const QString &, const QString &, QWidget *parent=0);
    ~Deskapp();
    void read_settings();
    void update_style();
    QString get_app_name() const;
    void set_selected(bool);

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void paintEvent(QPaintEvent *);

signals:
    void destroy_deskapp(Deskapp *);

public slots:
    void del_app();

private:
    QString app_name;
    QString app_exec;
    QString delete_link_pix;
    QPoint mousepos;
    QColor d_app_col;
    QPixmap d_app_pix;
    bool zoom;
    bool selected;
    QSettings *antico;
    QMenu *menu;
};

#endif

