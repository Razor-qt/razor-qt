////////////////////////////////////////
//  File      : deskdev.h             //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// device added on the desk

////////////////////////////////////////

#ifndef DESKDEV_H
#define DESKDEV_H

#include "defs.h"
#include "filedialog.h"
#include "desk.h"

class Filedialog;
class Desk;

////////////////////////////////////////

class Deskdev : public QWidget
{
    Q_OBJECT

public:
    Deskdev(Categorymenu *, const QString &, const QString &, const QString &, const QString &, const QString &, Desk *parent=0);
    ~Deskdev();
    void read_settings();
    void init();
    void update_style();

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void paintEvent(QPaintEvent *);
    void contextMenuEvent(QContextMenuEvent *);

signals:

public slots:
    void run_menu(QAction *);
 
private:
    QString device_name;
    QString mount_path;
    QString device_label;
    QString device_type;
    QString uuid;
    QPoint mousepos;
    QString d_disk_pix;
    QString d_cdrom_pix;
    QString open_with_pix;
    QColor d_dev_col;
    QPixmap dev_pix;
    bool zoom;
    QMenu *main_menu;
    QMenu *open_menu;
    Filedialog *file_dialog;
    Categorymenu *cat_menu;
    Desk *desktop;
};

#endif
