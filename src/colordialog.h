////////////////////////////////////////
//  File      : colordialog.h         //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// dialog for color selection

////////////////////////////////////////

#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include "defs.h"
#include "colorsel.h"

////////////////////////////////////////

class Colordialog : public QDialog
{
    Q_OBJECT

public:
    Colordialog(const QColor &, QWidget *parent=0);
    ~Colordialog();
    QColor get_color() const;
    void init();

protected:
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);

signals:

public slots:

private:
    QPoint mousepos;
    QColor actual_col;
    QDialogButtonBox *buttonBox;
    Colorsel *col_sel;
};


#endif
