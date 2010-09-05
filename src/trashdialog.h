////////////////////////////////////////
//  File      : trashdialog.h         //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// Trash dir/file contents (freedesktop.org implementation)

////////////////////////////////////////

#ifndef TRASHDIALOG_H
#define TRASHDIALOG_H

#include "defs.h"
#include "utils.h"
#include "msgbox.h"

////////////////////////////////////////

class Trashdialog : public QDialog
{
    Q_OBJECT

public:
    Trashdialog(QWidget *parent=0);
    ~Trashdialog();
    void read_settings();
    void init();
    int files_counter();

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);

signals:

public slots:
    void close_pressed();
    void delete_pressed();
    void restore_pressed();

private:
    QString ok_button_pix;
    QString close_button_pix;
    QString restore_button_pix;
    QString remove_button_pix;
    QPoint mousepos;
    QString trash_path;
    QString stl_path;
    QString stl_name;
    QLineEdit *line_path;
    QFileSystemModel *fs_model;
    QTreeView *tree_view;
    Fileicon *prov;
};

#endif
