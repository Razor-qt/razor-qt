////////////////////////////////////////
//  File      : deskfile.h            //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// file added on the desk

////////////////////////////////////////

#ifndef DESKFILE_H
#define DESKFILE_H

#include "defs.h"
#include "filedialog.h"
#include "utils.h"

////////////////////////////////////////

class Deskfile : public QWidget
{
    Q_OBJECT

public:
    Deskfile(Categorymenu *, const QString &, const QString &, const QString &, QWidget *parent=0);
    ~Deskfile();
    void read_settings();
    void init();
    void update_style();
    QString get_file_name() const;
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
    void destroy_deskfile(Deskfile *);
    
public slots:
    void del_file();

private:
    QString file_name;
    QString file_path;
    QString delete_link_pix;
    QString open_with_pix;
    QPoint mousepos;
    QColor d_file_col;
    QPixmap d_file_pix;
    bool zoom;
    bool selected;
    QMenu *main_menu;
    QMenu *open_menu;
    QSettings *antico;
    QAction *delete_file;
    Categorymenu *cat_menu;
};

#endif

