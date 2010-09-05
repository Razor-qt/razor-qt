////////////////////////////////////////
//  File      : deskfolder.h          //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// folder added on the desk

////////////////////////////////////////

#ifndef DESKFOLDER_H
#define DESKFOLDER_H

#include "defs.h"
#include "filedialog.h"

class Filedialog;

////////////////////////////////////////

class Deskfolder : public QWidget
{
    Q_OBJECT

public:
    Deskfolder(Categorymenu *, const QString &, const QString &, const QRect &, QWidget *parent=0);
    ~Deskfolder();
    void read_settings();
    void update_style();
    void init();
    QString get_dir_name() const;
    void set_selected(bool);

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
    void destroy_deskfolder(Deskfolder *);

public slots:
    void del_folder();

private:
    QString dir_name;
    QString dir_path;
    QRect geom;
    QPoint mousepos;
    QString d_folder_pix;
    QString delete_link_pix;
    QString open_with_pix;
    QColor d_folder_col;
    QPixmap pix;
    bool zoom;
    bool selected;
    QSettings *antico;
    QAction *delete_folder;
    QMenu *main_menu;
    QMenu *open_menu;
    Filedialog *file_dialog;
    Categorymenu *cat_menu;
};

#endif
