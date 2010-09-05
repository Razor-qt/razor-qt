////////////////////////////////////////
//  File      : dockapp.h             //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// add application link on the dockbar

////////////////////////////////////////

#ifndef DOCKAPP_H
#define DOCKAPP_H

#include "defs.h"
#include "filedialog.h"

////////////////////////////////////////

class Dockapp : public QLabel
{
    Q_OBJECT

public:
    Dockapp(const QString &, const QString &, const QString &, QWidget *parent=0);
    ~Dockapp();
    void read_settings();
    void update_style();

protected:
    void mousePressEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

signals:
    void destroy_dockapp(Dockapp *);

public slots:
    void del_app();

private:
    QString app_name;
    QString app_exec;
    QString delete_link_pix;
    QString d_app_pix;
    QPoint mousepos;
    int dock_height;
    QSettings *antico;
    QMenu *menu;
};

#endif

