////////////////////////////////////////
//  File      : trash.h               //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// Trash pixmap (freedesktop.org implementation)

////////////////////////////////////////

#ifndef TRASH_H
#define TRASH_H

#include "defs.h"
#include "utils.h"
#include "msgbox.h"
#include "trashdialog.h"

////////////////////////////////////////

class Trash : public QWidget
{
    Q_OBJECT

public:
    Trash(QWidget *parent=0);
    ~Trash();
    void read_settings();
    void update_style();
    void init();

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void paintEvent(QPaintEvent *);

signals:

public slots:

private:
    QString trash_pix;
    QColor trash_col;
    QPixmap pix;
    bool zoom;
    QPoint mousepos;
    QString stl_path;
    QString stl_name;
    QSettings *antico;
    Trashdialog *trash_dlg;
};

#endif
