////////////////////////////////////////
//  File      : runner.h              //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// start an application from command line

////////////////////////////////////////

#ifndef RUNNER_H
#define RUNNER_H

#include "defs.h"
#include "msgbox.h"

////////////////////////////////////////

class Runner : public QDialog
{
    Q_OBJECT

public:
    Runner(QWidget *parent=0);
    ~Runner();
    void read_settings();
    void init();
    void load_cmd_history();

protected:
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);

signals:

public slots:
    void run_pressed();
    void close_pressed();

private:
    QPoint mousepos;
    QString ok_button_pix_path;
    QString close_button_pix_path;
    QComboBox *command_line;
    QFile *run_history;
    QStringList history_list;
};


#endif
